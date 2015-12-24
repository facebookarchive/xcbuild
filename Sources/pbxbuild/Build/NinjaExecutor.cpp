/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Build/NinjaExecutor.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseInvocations.h>
#include <ninja/Writer.h>
#include <ninja/Value.h>

#include <fstream>

using pbxbuild::Build::NinjaExecutor;
using pbxbuild::BuildEnvironment;
using pbxbuild::BuildContext;
using pbxbuild::TargetEnvironment;
using pbxbuild::ToolInvocation;
using libutil::FSUtil;

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
TargetNinjaPath(pbxproj::PBX::Target::shared_ptr const &target, TargetEnvironment const &targetEnvironment)
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
    return "do";
}

static bool
WriteNinja(ninja::Writer const &writer, std::string const &path)
{
    std::string contents = writer.serialize();

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
    BuildEnvironment const &buildEnvironment,
    BuildContext const &buildContext,
    BuildGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph)
{
    /*
     * This environment contains only settings shared for the entire build.
     */
    pbxsetting::Environment environment = buildEnvironment.baseEnvironment();
    environment.insertFront(buildContext.baseSettings(), false);

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
    // TODO(grp): Add some comments about the workspace here.
    writer.comment("Action: " + buildContext.action());
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
    writer.rule(NinjaRuleName(), ninja::Value::Expression("$exec"));

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
        writer.build({ ninja::Value::String(targetBegin) }, "phony", { }, { }, dependenciesFinished);

        /*
         * Resolve this target and generate its Ninja file.
         */
        std::unique_ptr<TargetEnvironment> targetEnvironment = buildContext.targetEnvironment(buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment for %s\n", target->name().c_str());
            continue;
        }

        Phase::PhaseEnvironment phaseEnvironment = Phase::PhaseEnvironment(buildEnvironment, buildContext, target, *targetEnvironment);
        Phase::PhaseInvocations phaseInvocations = Phase::PhaseInvocations::Create(phaseEnvironment, target);

        auto result = buildTarget(target, *targetEnvironment, phaseInvocations.invocations());
        if (!result.first) {
            return false;
        }

        /*
         * Load the Ninja file generated for this target.
         */
        std::string targetPath = TargetNinjaPath(target, *targetEnvironment);
        writer.subninja(targetPath);

        /*
         * As described above, the target's finish depends on all of the invocation outputs.
         */
        std::vector<ninja::Value> invocationOutputs;
        for (ToolInvocation const &invocation : phaseInvocations.invocations()) {
            for (std::string const &output : invocation.outputs()) {
                invocationOutputs.push_back(ninja::Value::String(output));
            }
        }

        /*
         * Add the phony target for ending this target's build.
         */
        std::string targetFinish = TargetNinjaFinish(target);
        writer.build({ ninja::Value::String(targetFinish) }, "phony", { }, { }, invocationOutputs);
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
ShellEscape(std::string const &value)
{
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string digits = "0123456789";

    if (value.find_first_not_of(alphabet + digits + "@%_-+=:,./") == std::string::npos) {
        return value;
    } else {
        std::string result = value;
        std::string::size_type offset = 0;
        while ((offset = result.find("'", offset)) != std::string::npos) {
            result.replace(offset, 1, "'\"'\"'");
            offset += 5;
        }
        return "'" + result + "'";
    }
}

static std::string
ResolveExecutable(std::string const &executable, std::vector<std::string> const &searchPaths)
{
    std::string builtinPrefix = "builtin-";
    bool builtin = executable.compare(0, builtinPrefix.size(), builtinPrefix) == 0;

    if (builtin) {
        // TODO(grp): Find the path to the builtin tool.
        return std::string();
    } else if (!FSUtil::IsAbsolutePath(executable)) {
        return FSUtil::FindExecutable(executable, searchPaths);
    } else {
        return executable;
    }
}

std::pair<bool, std::vector<ToolInvocation const>> NinjaExecutor::
buildTarget(
    pbxproj::PBX::Target::shared_ptr const &target,
    TargetEnvironment const &targetEnvironment,
    std::vector<ToolInvocation const> const &invocations
)
{
    // TODO(grp): Handle auxiliary files and creating the directory structure.
    std::string targetBegin = TargetNinjaBegin(target);

    /*
     * Start building the Ninja file for this target.
     */
    ninja::Writer writer;
    writer.comment("xcbuild ninja");
    writer.comment("Target: " + target->name());
    writer.newline();

    for (ToolInvocation const &invocation : invocations) {
        // TODO(grp): This should perhaps be a separate flag for a 'phony' invocation.
        if (invocation.executable().empty()) {
            continue;
        }

        /*
         * Find the executable to use for the invocation. For builtin tools, this will return
         * a path to a real executable that Ninja can execute, rather than doing it in-process.
         */
        std::string executable = ResolveExecutable(invocation.executable(), targetEnvironment.sdk()->executablePaths());
        if (executable.empty()) {
            fprintf(stderr, "error: unable to find executable %s\n", invocation.executable().c_str());
            continue;
        }

        /*
         * Build the invocation arguments. Must escape for shell arguments as Ninja passes
         * the command string directly to the shell, which would interpret spaces, etc as meaningful.
         */
        std::string exec = ShellEscape(executable);
        for (std::string const &arg : invocation.arguments()) {
            exec += " " + ShellEscape(arg);
        }

        /*
         * Determine the status message for Ninja to print for this invocation.
         */
        std::string description = _formatter->beginInvocation(invocation, executable);

        /* Limit to the first line: Ninja can only handle a single line status. */
        std::string::size_type newline = description.find('\n');
        if (newline != std::string::npos) {
            description = description.substr(0, description.find('\n'));
        }

        /*
         * Build up the bindings for the invocation.
         */
        std::vector<ninja::Binding> bindings = {
            { "description", ninja::Value::String(description) },
            { "exec", ninja::Value::String(exec) },
        };

#if 0
        // TODO(grp): Two issues here.
        //  1. "-MT dependencies" doesn't work with Ninja.
        //  2. ld64 dependency files are some other format, not a Makefile.
        if (!invocation.dependencyInfo().empty()) {
            bindings.push_back({ "depfile", ninja::Value::String(invocation.dependencyInfo()) });
        }
#endif

        /*
         * Build up outputs as literal Ninja values.
         */
        std::vector<ninja::Value> outputs;
        for (std::string const &output : invocation.outputs()) {
            outputs.push_back(ninja::Value::String(output));
        }

        /*
         * Add phony rules for input dependencies that we don't know if they exist.
         * This can come up, for example, for user-specified custom script inputs.
         */
        for (std::string const &phonyInput : invocation.phonyInputs()) {
            writer.build({ ninja::Value::String(phonyInput) }, "phony", { });
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
         * All invocations depend on the target containing them beginning.
         */
        inputDependencies.push_back(ninja::Value::String(targetBegin));

        /*
         * Build up order dependencies as literal Ninja values.
         */
        std::vector<ninja::Value> orderDependencies;
        for (std::string const &orderDependency : invocation.orderDependencies()) {
            orderDependencies.push_back(ninja::Value::String(orderDependency));
        }

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
        return std::make_pair(false, std::vector<ToolInvocation const>());;
    }

    /*
     * Note where the Ninja file is written.
     */
    fprintf(stderr, "Wrote %s ninja: %s\n", target->name().c_str(), path.c_str());

    return std::make_pair(true, std::vector<ToolInvocation const>());
}

std::unique_ptr<NinjaExecutor> NinjaExecutor::
Create(std::shared_ptr<Formatter> const &formatter, bool dryRun)
{
    return std::unique_ptr<NinjaExecutor>(new NinjaExecutor(
        formatter,
        dryRun
    ));
}
