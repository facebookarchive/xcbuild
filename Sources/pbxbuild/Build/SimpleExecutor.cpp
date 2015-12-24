/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Build/SimpleExecutor.h>
#include <pbxbuild/BuildGraph.h>

#include <fstream>

using pbxbuild::Build::SimpleExecutor;
using pbxbuild::BuildEnvironment;
using pbxbuild::BuildContext;
using pbxbuild::ToolInvocation;
using pbxbuild::BuildGraph;
using libutil::FSUtil;
using libutil::Subprocess;

SimpleExecutor::
SimpleExecutor(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun, builtin::Registry const &builtins) :
    Executor (buildEnvironment, buildContext, formatter, dryRun),
    _builtins(builtins)
{
}

SimpleExecutor::
~SimpleExecutor()
{
}

void SimpleExecutor::
prepare()
{
}

void SimpleExecutor::
finish()
{
}

static std::vector<ToolInvocation const>
SortInvocations(std::vector<ToolInvocation const> const &invocations)
{
    std::unordered_map<std::string, ToolInvocation const *> outputToInvocation;
    for (ToolInvocation const &invocation : invocations) {
        for (std::string const &output : invocation.outputs()) {
            outputToInvocation.insert({ output, &invocation });
        }
        for (std::string const &outputDependency : invocation.outputDependencies()) {
            outputToInvocation.insert({ outputDependency, &invocation });
        }
    }

    BuildGraph<ToolInvocation const *> graph;
    for (ToolInvocation const &invocation : invocations) {
        graph.insert(&invocation, { });

        for (std::string const &input : invocation.inputs()) {
            auto it = outputToInvocation.find(input);
            if (it != outputToInvocation.end()) {
                graph.insert(&invocation, { it->second });
            }
        }
        for (std::string const &inputDependency : invocation.inputDependencies()) {
            auto it = outputToInvocation.find(inputDependency);
            if (it != outputToInvocation.end()) {
                graph.insert(&invocation, { it->second });
            }
        }
    }

    std::vector<ToolInvocation const> result;
    for (ToolInvocation const *invocation : graph.ordered()) {
        result.push_back(*invocation);
    }
    return result;
}

bool SimpleExecutor::
buildTarget(
    pbxproj::PBX::Target::shared_ptr const &target,
    TargetEnvironment const &targetEnvironment,
    std::vector<ToolInvocation const> const &invocations
)
{
    Formatter::Print(_formatter->beginWriteAuxiliaryFiles(target));
    for (ToolInvocation const &invocation : invocations) {
        for (std::string const &output : invocation.outputs()) {
            std::string directory = FSUtil::GetDirectoryName(output);
            if (!FSUtil::TestForDirectory(directory)) {
                Formatter::Print(_formatter->createAuxiliaryDirectory(directory));

                if (!_dryRun) {
                    Subprocess process;
                    if (!process.execute("/bin/mkdir", { "-p", directory }) || process.exitcode() != 0) {
                        Formatter::Print(_formatter->failure(_buildContext, { }));
                        return false;
                    }
                }
            }
        }

        for (ToolInvocation::AuxiliaryFile const &auxiliaryFile : invocation.auxiliaryFiles()) {
            // TODO(grp): This is incorrect if the file is already written, need to check modification times.
            if (!FSUtil::TestForRead(auxiliaryFile.path())) {
                Subprocess process;
                if (!process.execute("/bin/mkdir", { "-p", FSUtil::GetDirectoryName(auxiliaryFile.path()) }) || process.exitcode() != 0) {
                    Formatter::Print(_formatter->failure(_buildContext, { }));
                    return false;
                }

                Formatter::Print(_formatter->writeAuxiliaryFile(auxiliaryFile.path()));

                if (!_dryRun) {
                    std::ofstream out;
                    out.open(auxiliaryFile.path(), std::ios::out | std::ios::trunc | std::ios::binary);
                    out.write(auxiliaryFile.contents().data(), auxiliaryFile.contents().size() * sizeof(char));
                    out.close();
                }

                if (auxiliaryFile.executable() && !FSUtil::TestForExecute(auxiliaryFile.path())) {
                    Formatter::Print(_formatter->setAuxiliaryExecutable(auxiliaryFile.path()));

                    if (!_dryRun) {
                        Subprocess process;
                        if (!process.execute("/bin/chmod", { "0755", auxiliaryFile.path() }) || process.exitcode() != 0) {
                            Formatter::Print(_formatter->failure(_buildContext, { }));
                            return false;
                        }
                    }
                }
            }
        }
    }
    Formatter::Print(_formatter->finishWriteAuxiliaryFiles(target));

    Formatter::Print(_formatter->beginCreateProductStructure(target));
    // TODO(grp): Create product structure.
    Formatter::Print(_formatter->finishCreateProductStructure(target));

    std::vector<ToolInvocation const> orderedInvocations = SortInvocations(invocations);
    for (ToolInvocation const &invocation : orderedInvocations) {
        // TODO(grp): This should perhaps be a separate flag for a 'phony' invocation.
        if (invocation.executable().empty()) {
            continue;
        }

        std::map<std::string, std::string> sortedEnvironment = std::map<std::string, std::string>(invocation.environment().begin(), invocation.environment().end());

        std::string executable = invocation.executable();

        std::string builtinPrefix = "builtin-";
        bool builtin = executable.compare(0, builtinPrefix.size(), builtinPrefix) == 0;

        if (!builtin && !FSUtil::IsAbsolutePath(executable)) {
            executable = FSUtil::FindExecutable(executable, targetEnvironment.sdk()->executablePaths());
            if (executable.empty()) {
                fprintf(stderr, "error: unable to find executable %s\n", invocation.executable().c_str());
            }
        }

        Formatter::Print(_formatter->beginInvocation(invocation, executable));

        if (!_dryRun) {
            for (std::string const &output : invocation.outputs()) {
                std::string directory = FSUtil::GetDirectoryName(output);

                if (!FSUtil::TestForDirectory(directory)) {
                    Subprocess process;
                    if (!process.execute("/bin/mkdir", { "-p", directory }) || process.exitcode() != 0) {
                        Formatter::Print(_formatter->finishInvocation(invocation, executable));
                        Formatter::Print(_formatter->failure(_buildContext, { invocation }));
                        return false;
                    }
                }
            }

            if (builtin) {
                std::shared_ptr<builtin::Driver> driver = _builtins.driver(executable);
                if (driver == nullptr) {
                    Formatter::Print(_formatter->finishInvocation(invocation, executable));
                    Formatter::Print(_formatter->failure(_buildContext, { invocation }));
                    return false;
                }

                if (driver->run(invocation.arguments(), invocation.environment(), invocation.workingDirectory()) != 0) {
                    Formatter::Print(_formatter->finishInvocation(invocation, executable));
                    Formatter::Print(_formatter->failure(_buildContext, { invocation }));
                    return false;
                }
            } else {
                Subprocess process;
                if (!process.execute(executable, invocation.arguments(), invocation.environment(), invocation.workingDirectory()) || process.exitcode() != 0) {
                    Formatter::Print(_formatter->finishInvocation(invocation, executable));
                    Formatter::Print(_formatter->failure(_buildContext, { invocation }));
                    return false;
                }
            }
        }

        Formatter::Print(_formatter->finishInvocation(invocation, executable));
    }

    return true;
}

std::unique_ptr<SimpleExecutor> SimpleExecutor::
Create(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun, builtin::Registry const &builtins)
{
    return std::unique_ptr<SimpleExecutor>(new SimpleExecutor(
        buildEnvironment,
        buildContext,
        formatter,
        dryRun,
        builtins
    ));
}
