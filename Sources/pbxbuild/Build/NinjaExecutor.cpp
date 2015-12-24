/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Build/NinjaExecutor.h>
#include <pbxbuild/BuildGraph.h>

#include <ninja/Writer.h>
#include <ninja/Value.h>

#include <fstream>

using pbxbuild::Build::NinjaExecutor;
using pbxbuild::BuildEnvironment;
using pbxbuild::BuildContext;
using pbxbuild::ToolInvocation;
using pbxbuild::BuildGraph;
using libutil::FSUtil;
using libutil::Subprocess;

NinjaExecutor::
NinjaExecutor(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun) :
    Executor(buildEnvironment, buildContext, formatter, dryRun)
{
}

NinjaExecutor::
~NinjaExecutor()
{
}

void NinjaExecutor::
prepare()
{
}

void NinjaExecutor::
finish()
{
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

bool NinjaExecutor::
buildTarget(
    pbxproj::PBX::Target::shared_ptr const &target,
    TargetEnvironment const &targetEnvironment,
    std::vector<ToolInvocation const> const &invocations
)
{
    // TODO(grp): Handle auxiliary files and creating the directory structure.

    /*
     * Determine where the Ninja file should go. We use the target's temp dir
     * as, being target-specific, it will allow the Ninja files to not conflict.
     */
    pbxsetting::Environment const &environment = targetEnvironment.environment();
    std::string root = environment.resolve("TARGET_TEMP_DIR");

    /*
     * Start building the Ninja file for this target.
     */
    ninja::Writer writer;
    writer.comment("xcbuild ninja");
    writer.comment("Target: " + target->name());
    writer.newline();

    /*
     * Ninja's intermediate outputs should also go in the temp dir.
     */
    writer.binding({ "builddir", { ninja::Value::String(root) } });

    /*
     * Since invocations are already resolved at this point, we can't use more specific
     * rules at the Ninja level. Instead, add a single rule that just passes through from
     * the build command that calls it.
     */
    std::string ruleName = "do";
    writer.rule(ruleName, ninja::Value::Expression("$exec"));

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

        // TODO(grp): Include all outputs.
        if (!invocation.outputs().size()) { continue; }
        ninja::Value output = invocation.outputs().size() ? ninja::Value::String(invocation.outputs().front()) : ninja::Value::Empty();

        /*
         * Add phony rules for input dependencies that we don't know if they exist.
         * This can come up, for example, for user-specified custom script inputs.
         */
        for (std::string const &phonyInput : invocation.phonyInputs()) {
            writer.build(ninja::Value::String(phonyInput), "phony", { });
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
         * Add the rule to build this invocation.
         */
        writer.build(output, ruleName, inputs, bindings, inputDependencies, orderDependencies);
    }

    /*
     * Serialize the Ninja file into the build root.
     */
    std::string contents = writer.serialize();
    std::string path = root + "/" + "build.ninja";

    std::ofstream out;
    out.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
    out.write(contents.data(), contents.size() * sizeof(char));
    out.close();

    /*
     * Note where the Ninja file is written.
     */
    Formatter::Print("Wrote " + target->name() + " ninja: " + path + "\n");

    return true;
}

std::unique_ptr<NinjaExecutor> NinjaExecutor::
Create(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun)
{
    return std::unique_ptr<NinjaExecutor>(new NinjaExecutor(
        buildEnvironment,
        buildContext,
        formatter,
        dryRun
    ));
}
