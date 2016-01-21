/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Action/NinjaExecutor.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/PhaseInvocations.h>
#include <ninja/Writer.h>
#include <ninja/Value.h>
#include <libutil/Escape.h>
#include <libutil/FSUtil.h>
#include <libutil/SysUtil.h>
#include <libutil/md5.h>

#include <sstream>
#include <iomanip>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

using pbxbuild::Action::NinjaExecutor;
namespace Build = pbxbuild::Build;
namespace Target = pbxbuild::Target;
namespace Tool = pbxbuild::Tool;
using libutil::Escape;
using libutil::FSUtil;
using libutil::SysUtil;

NinjaExecutor::
NinjaExecutor(std::shared_ptr<Formatter> const &formatter, bool dryRun) :
    Executor(formatter, dryRun)
{
}

NinjaExecutor::
~NinjaExecutor()
{
}

static std::string
TargetNinjaBegin(pbxproj::PBX::Target::shared_ptr const &target)
{
    return "begin-target-" + target->name();
}

static std::string
TargetNinjaFinish(pbxproj::PBX::Target::shared_ptr const &target)
{
    return "finish-target-" + target->name();
}

static std::string
TargetNinjaPath(pbxproj::PBX::Target::shared_ptr const &target, Target::Environment const &targetEnvironment)
{
    /*
     * Determine where the Ninja file should go. We use the target's temp dir
     * as, being target-specific, it will allow the Ninja files to not conflict.
     */
    pbxsetting::Environment const &environment = targetEnvironment.environment();
    std::string temporaryDirectory = environment.resolve("TARGET_TEMP_DIR");
    // TODO(grp): How to handle varying configurations / actions / other build context options?

    return temporaryDirectory + "/" + "build.ninja";
}

static std::string
NinjaRuleName()
{
    return "invoke";
}

static std::string
NinjaDescription(std::string const &description)
{
    /* Limit to the first line: Ninja can only handle a single line status. */
    std::string::size_type newline = description.find('\n');
    if (newline != std::string::npos) {
        return description.substr(0, description.find('\n'));
    } else {
        return description;
    }
}

static std::string
NinjaHash(std::string const &input)
{
    md5_state_t state;
    md5_init(&state);
    md5_append(&state, reinterpret_cast<const md5_byte_t *>(input.data()), input.size());
    uint8_t digest[16];
    md5_finish(&state, reinterpret_cast<md5_byte_t *>(&digest));

    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t c : digest) {
        ss << std::setw(2) << static_cast<int>(c);
    }

    return ss.str();
}

static std::string
NinjaInvocationPhonyOutput(Tool::Invocation const &invocation)
{
    /*
     * This is a hack to support invocations that have no outputs. Ninja requires
     * all targets to have an output, but in some cases (usually with build script
     * invocations), the invocation has no outputs.
     *
     * In that case, generate a phony output name that will never exist, so the
     * target will always be out of date. This is expected, since with no outputs
     * there's no way to tell if the invocation needs to run.
     */

    // TODO(grp): Handle identical phony output invocations in a build.
    std::string key = invocation.executable().path();
    for (std::string const &arg : invocation.arguments()) {
        key += " " + arg;
    }

    return ".ninja-phony-output-" + NinjaHash(key);
}

static std::vector<std::string>
NinjaInvocationOutputs(Tool::Invocation const &invocation)
{
    std::vector<std::string> outputs;

    if (!invocation.outputs().empty()) {
        outputs.insert(outputs.end(), invocation.outputs().begin(), invocation.outputs().end());
    } else {
        outputs.push_back(NinjaInvocationPhonyOutput(invocation));
    }

    return outputs;
}

static bool
WriteNinja(ninja::Writer const &writer, std::string const &path)
{
    std::string contents = writer.serialize();

    if (!FSUtil::CreateDirectory(FSUtil::GetDirectoryName(path))) {
        return false;
    }

    std::ofstream out;
    out.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (out.fail()) {
        return false;
    }

    out.write(contents.data(), contents.size() * sizeof(char));
    out.close();

    return true;
}

bool NinjaExecutor::
build(
    Build::Environment const &buildEnvironment,
    Build::Context const &buildContext,
    DirectedGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph)
{
    /*
     * This environment contains only settings shared for the entire build.
     */
    pbxsetting::Environment environment = buildEnvironment.baseEnvironment();
    environment.insertFront(buildContext.baseSettings(), false);
    environment.insertFront(buildContext.actionSettings(), false);

    /*
     * Determine where build-level outputs will go. Note we can't use CONFIGURATION_BUILD_DIR
     * at this point because that includes the EFFECTIVE_PLATFORM_NAME, but we don't have a platform.
     */
    std::string intermediatesDirectory = environment.resolve("OBJROOT");
    // TODO(grp): How to handle varying configurations / actions / other build context options?

    /*
     * Write out a Ninja file for the build as a whole. Note each target will have a separate
     * file, this is to coordinate the build between targets.
     */
    ninja::Writer writer;
    writer.comment("xcbuild ninja");
    writer.comment("Action: " + buildContext.action());
    if (buildContext.workspaceContext().workspace() != nullptr) {
        writer.comment("Workspace: " + buildContext.workspaceContext().workspace()->projectFile());
    } else if (buildContext.workspaceContext().project() != nullptr) {
        writer.comment("Project: " + buildContext.workspaceContext().project()->projectFile());
    }
    if (buildContext.scheme() != nullptr) {
        writer.comment("Scheme: " + buildContext.scheme()->name());
    }
    writer.comment("Configuation: " + buildContext.configuration());
    writer.newline();

    /*
     * Ninja's intermediate outputs should also go in the temp dir.
     */
    writer.binding({ "builddir", { ninja::Value::String(intermediatesDirectory) } });
    writer.newline();

    /*
     * Since invocations are already resolved at this point, we can't use more specific
     * rules at the Ninja level. Instead, add a single rule that just passes through from
     * the build command that calls it.
     */
    writer.rule(NinjaRuleName(), ninja::Value::Expression("cd $dir && env -i $env $exec && $depexec"));

    /*
     * Go over each target and write out Ninja targets for the start and end of each.
     * Don't bother topologically sorting the targets now, since Ninja will do that for us.
     */
    for (pbxproj::PBX::Target::shared_ptr const &target : targetGraph.nodes()) {

        /*
         * Beginning target depends on finishing the targets before that. This is implemented
         * in three parts:
         *
         *  1. Each target has a "target begin" Ninja target depending on completing the build
         *     of any dependent targets.
         *  2. Each invocation's Ninja target depends on the "target begin" target to order
         *     them necessarily after the target started building.
         *  3. Each target also has a "target finish" Ninja target, which depends on all of
         *     the invocations created for the target.
         *
         * The end result is that targets build in the right order. Note this does not preclude
         * cross-target parallelization; if the target dependency graph doesn't have an edge,
         * then they will be parallelized. Linear builds have edges from each target to all
         * previous targets.
         */

        /*
         * Resolve this target and generate its invocations.
         */
        ext::optional<Target::Environment> targetEnvironment = buildContext.targetEnvironment(buildEnvironment, target);
        if (!targetEnvironment) {
            fprintf(stderr, "error: couldn't create target environment for %s\n", target->name().c_str());
            continue;
        }

        Phase::Environment phaseEnvironment = Phase::Environment(buildEnvironment, buildContext, target, *targetEnvironment);
        Phase::PhaseInvocations phaseInvocations = Phase::PhaseInvocations::Create(phaseEnvironment, target);

        /*
         * As described above, the target's begin depends on all of the target dependencies.
         */
        std::vector<ninja::Value> dependenciesFinished;
        for (pbxproj::PBX::Target::shared_ptr const &dependency : targetGraph.adjacent(target)) {
            std::string targetFinished = TargetNinjaFinish(dependency);
            dependenciesFinished.push_back(ninja::Value::String(targetFinished));
        }

        /*
         * Add the phony target for beginning this target's build.
         */
        std::string targetBegin = TargetNinjaBegin(target);
        writer.build({ ninja::Value::String(targetBegin) }, "phony", dependenciesFinished);

        /*
         * Write out the Ninja file to build this target.
         */
        if (!buildTargetInvocations(target, *targetEnvironment, phaseInvocations.invocations())) {
            return false;
        }

        /*
         * Load the Ninja file generated for this target.
         */
        std::string targetPath = TargetNinjaPath(target, *targetEnvironment);
        writer.subninja(ninja::Value::String(targetPath));

        /*
         * As described above, the target's finish depends on all of the invocation outputs.
         */
        std::unordered_set<std::string> invocationOutputs;
        for (Tool::Invocation const &invocation : phaseInvocations.invocations()) {
            if (invocation.executable().path().empty()) {
                /* No outputs. */
                continue;
            }

            std::vector<std::string> outputs = NinjaInvocationOutputs(invocation);
            invocationOutputs.insert(outputs.begin(), outputs.end());
        }

        /*
         * Add phony rules for input dependencies that we don't know if they exist.
         * This can come up, for example, for user-specified custom script inputs.
         * However, avoid adding the phony invocation if a real output *does* include
         * the phony input, to avoid Ninja complaining about duplicate rules.
         */
        for (Tool::Invocation const &invocation : phaseInvocations.invocations()) {
            for (std::string const &phonyInput : invocation.phonyInputs()) {
                if (invocationOutputs.find(phonyInput) == invocationOutputs.end()) {
                    writer.build({ ninja::Value::String(phonyInput) }, "phony", { });
                }
            }
        }

        /*
         * Add the phony target for ending this target's build.
         */
        std::string targetFinish = TargetNinjaFinish(target);
        std::vector<ninja::Value> invocationOutputsValues;
        for (std::string const &output : invocationOutputs) {
            invocationOutputsValues.push_back(ninja::Value::String(output));
        }
        writer.build({ ninja::Value::String(targetFinish) }, "phony", { }, { }, invocationOutputsValues);
    }

    /*
     * Serialize the Ninja file into the build root.
     */
    std::string path = intermediatesDirectory + "/" + "build.ninja";
    if (!WriteNinja(writer, path)) {
        return false;
    }

    /*
     * Note where the Ninja file is written.
     */
    fprintf(stderr, "Wrote meta-ninja: %s\n", path.c_str());

    return true;
}

static std::string
LocalExecutable(std::string const &executable)
{
    std::string executableRoot = FSUtil::GetDirectoryName(SysUtil::GetExecutablePath());
    return executableRoot + "/" + executable;
}

static std::string
NinjaDependencyInfoExecutable()
{
    return LocalExecutable("ninja-dependency-info");
}

bool NinjaExecutor::
buildTargetAuxiliaryFiles(
    ninja::Writer *writer,
    pbxproj::PBX::Target::shared_ptr const &target,
    Target::Environment const &targetEnvironment,
    std::vector<Tool::Invocation const> const &invocations)
{
    // TODO(grp): In a dry run, Ninja will still need these files to exist, but the whole
    // point of a dry run is to avoid the filesystem. What's the best way to resolve this?
    if (_dryRun) {
        return true;
    }

    // TODO(grp): Could this defer writing auxiliary files and let Ninja do it?
    for (Tool::Invocation const &invocation : invocations) {
        for (Tool::Invocation::AuxiliaryFile const &auxiliaryFile : invocation.auxiliaryFiles()) {
            if (!FSUtil::CreateDirectory(FSUtil::GetDirectoryName(auxiliaryFile.path()))) {
                return false;
            }

            std::ofstream out;
            out.open(auxiliaryFile.path(), std::ios::out | std::ios::trunc | std::ios::binary);
            if (out.fail()) {
                return false;
            }

            std::copy(auxiliaryFile.contents().begin(), auxiliaryFile.contents().end(), std::ostream_iterator<char>(out));
            out.close();

            if (auxiliaryFile.executable() && !FSUtil::TestForExecute(auxiliaryFile.path())) {
                if (::chmod(auxiliaryFile.path().c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool NinjaExecutor::
buildTargetInvocations(
    pbxproj::PBX::Target::shared_ptr const &target,
    Target::Environment const &targetEnvironment,
    std::vector<Tool::Invocation const> const &invocations)
{
    std::string targetBegin = TargetNinjaBegin(target);

    /*
     * Start building the Ninja file for this target.
     */
    ninja::Writer writer;
    writer.comment("xcbuild ninja");
    writer.comment("Target: " + target->name());
    writer.newline();

    /*
     * Write out auxiliary files used by the invocations.
     */
    if (!buildTargetAuxiliaryFiles(&writer, target, targetEnvironment, invocations)) {
        return false;
    }

    /*
     * Add the build command for each invocation.
     */
    for (Tool::Invocation const &invocation : invocations) {
        // TODO(grp): This should perhaps be a separate flag for a 'phony' invocation.
        if (invocation.executable().path().empty()) {
            continue;
        }

        /*
         * Build the invocation arguments. Must escape for shell arguments as Ninja passes
         * the command string directly to the shell, which would interpret spaces, etc as meaningful.
         */
        std::string exec = Escape::Shell(invocation.executable().path());
        for (std::string const &arg : invocation.arguments()) {
            exec += " " + Escape::Shell(arg);
        }

        /*
         * Build the invocation environment. To set the environment, we use standard shell syntax.
         * Use `env` to avoid Bash-specific limitations on environment variables. Specifically, some
         * versions of Bash don't allow setting "UID". Pass -i to clear out the environment.
         */
        std::string environment;
        for (auto it = invocation.environment().begin(); it != invocation.environment().end(); ++it) {
            if (it != invocation.environment().begin()) {
                environment += " ";
            }
            environment += it->first + "=" + Escape::Shell(it->second);
        }

        /*
         * Determine the status message for Ninja to print for this invocation.
         */
        std::string description = NinjaDescription(_formatter->beginInvocation(invocation, invocation.executable().displayName(), false));

        /*
         * Add the dependency info converter & file.
         */
        std::string dependencyInfoFile;
        std::string dependencyInfoExec;

        if (!invocation.dependencyInfo().empty()) {
            /* Determine the first output; Ninja expects that as the Makefile rule. */
            std::string output = NinjaInvocationOutputs(invocation).front();

            /* Find where the generated dependency info should go. */
            pbxsetting::Environment const &environment = targetEnvironment.environment();
            std::string temporaryDirectory = environment.resolve("TARGET_TEMP_DIR");
            dependencyInfoFile = temporaryDirectory + "/" + ".ninja-dependency-info-" + NinjaHash(output) + ".d";

            /* Build the dependency info rewriter arguments. */
            std::string dependencyInfoExecutable = NinjaDependencyInfoExecutable();
            std::vector<std::string> dependencyInfoArguments = {
                "--name", output,
                "--output", dependencyInfoFile,
            };

            /* Add the input for each dependency info. */
            for (Tool::Invocation::DependencyInfo const &dependencyInfo : invocation.dependencyInfo()) {
                std::string formatName;
                if (!dependency::DependencyInfoFormats::Name(dependencyInfo.format(), &formatName)) {
                    return false;
                }

                dependencyInfoArguments.push_back(formatName + ":" + dependencyInfo.path());
            }

            /* Create the command for converting the dependency info. */
            dependencyInfoExec = Escape::Shell(dependencyInfoExecutable);
            for (std::string const &arg : dependencyInfoArguments) {
                dependencyInfoExec += " " + Escape::Shell(arg);
            }
        } else {
            // TODO(grp): Avoid the need for an empty dependency info command if not used.
            dependencyInfoExec = "true";
        }

        /*
         * Build up the bindings for the invocation.
         */
        std::vector<ninja::Binding> bindings = {
            { "description", ninja::Value::String(description) },
            { "dir", ninja::Value::String(Escape::Shell(invocation.workingDirectory())) },
            { "exec", ninja::Value::String(exec) },
        };
        if (!environment.empty()) {
            bindings.push_back({ "env", ninja::Value::String(environment) });
        }
        if (!dependencyInfoExec.empty()) {
            bindings.push_back({ "depexec", ninja::Value::String(dependencyInfoExec) });
        }
        if (!dependencyInfoFile.empty()) {
            bindings.push_back({ "depfile", ninja::Value::String(dependencyInfoFile) });
        }

        /*
         * Build up outputs as literal Ninja values.
         */
        std::vector<ninja::Value> outputs;
        for (std::string const &output : NinjaInvocationOutputs(invocation)) {
            outputs.push_back(ninja::Value::String(output));
        }

        /*
         * Build up inputs as literal Ninja values.
         */
        std::vector<ninja::Value> inputs;
        for (std::string const &input : invocation.inputs()) {
            inputs.push_back(ninja::Value::String(input));
        }

        /*
         * Build up input dependencies as literal Ninja values.
         */
        std::vector<ninja::Value> inputDependencies;
        for (std::string const &inputDependency : invocation.inputDependencies()) {
            inputDependencies.push_back(ninja::Value::String(inputDependency));
        }

        /*
         * Build up order dependencies as literal Ninja values.
         */
        std::vector<ninja::Value> orderDependencies;
        for (std::string const &orderDependency : invocation.orderDependencies()) {
            orderDependencies.push_back(ninja::Value::String(orderDependency));
        }

        /*
         * All invocations depend on the target containing them beginning.
         */
        orderDependencies.push_back(ninja::Value::String(targetBegin));

        /*
         * Add the rule to build this invocation.
         */
        writer.build(outputs, NinjaRuleName(), inputs, bindings, inputDependencies, orderDependencies);
    }

    /*
     * Serialize the Ninja file into the build root.
     */
    std::string path = TargetNinjaPath(target, targetEnvironment);
    if (!WriteNinja(writer, path)) {
        return false;
    }

    /*
     * Note where the Ninja file is written.
     */
    fprintf(stderr, "Wrote %s ninja: %s\n", target->name().c_str(), path.c_str());

    return true;
}

std::unique_ptr<NinjaExecutor> NinjaExecutor::
Create(std::shared_ptr<Formatter> const &formatter, bool dryRun)
{
    return std::unique_ptr<NinjaExecutor>(new NinjaExecutor(
        formatter,
        dryRun
    ));
}
