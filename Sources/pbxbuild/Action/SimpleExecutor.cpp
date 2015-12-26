/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Action/SimpleExecutor.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/PhaseInvocations.h>

#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

using pbxbuild::Action::SimpleExecutor;
namespace Tool = pbxbuild::Tool;
namespace Build = pbxbuild::Build;
using pbxbuild::DirectedGraph;
using libutil::FSUtil;
using libutil::Subprocess;

SimpleExecutor::
SimpleExecutor(std::shared_ptr<Formatter> const &formatter, bool dryRun, builtin::Registry const &builtins) :
    Executor (formatter, dryRun),
    _builtins(builtins)
{
}

SimpleExecutor::
~SimpleExecutor()
{
}

bool SimpleExecutor::
build(
    Build::Environment const &buildEnvironment,
    Build::Context const &buildContext,
    DirectedGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph)
{
    Formatter::Print(_formatter->begin(buildContext));

    for (pbxproj::PBX::Target::shared_ptr const &target : targetGraph.ordered()) {
        Formatter::Print(_formatter->beginTarget(buildContext, target));

        std::unique_ptr<Target::Environment> targetEnvironment = buildContext.targetEnvironment(buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment for %s\n", target->name().c_str());
            Formatter::Print(_formatter->finishTarget(buildContext, target));
            continue;
        }

        Formatter::Print(_formatter->beginCheckDependencies(target));
        Phase::Environment phaseEnvironment = Phase::Environment(buildEnvironment, buildContext, target, *targetEnvironment);
        Phase::PhaseInvocations phaseInvocations = Phase::PhaseInvocations::Create(phaseEnvironment, target);
        Formatter::Print(_formatter->finishCheckDependencies(target));

        auto result = buildTarget(target, *targetEnvironment, phaseInvocations.invocations());
        if (!result.first) {
            Formatter::Print(_formatter->finishTarget(buildContext, target));
            Formatter::Print(_formatter->failure(buildContext, result.second));
            return false;
        }

        Formatter::Print(_formatter->finishTarget(buildContext, target));
    }

    Formatter::Print(_formatter->success(buildContext));
    return true;
}

static std::vector<Tool::Invocation const>
SortInvocations(std::vector<Tool::Invocation const> const &invocations)
{
    std::unordered_map<std::string, Tool::Invocation const *> outputToInvocation;
    for (Tool::Invocation const &invocation : invocations) {
        for (std::string const &output : invocation.outputs()) {
            outputToInvocation.insert({ output, &invocation });
        }
        for (std::string const &phonyOutput : invocation.phonyOutputs()) {
            outputToInvocation.insert({ phonyOutput, &invocation });
        }
        for (std::string const &outputDependency : invocation.outputDependencies()) {
            outputToInvocation.insert({ outputDependency, &invocation });
        }
    }

    DirectedGraph<Tool::Invocation const *> graph;
    for (Tool::Invocation const &invocation : invocations) {
        graph.insert(&invocation, { });

        for (std::string const &input : invocation.inputs()) {
            auto it = outputToInvocation.find(input);
            if (it != outputToInvocation.end()) {
                graph.insert(&invocation, { it->second });
            }
        }
        for (std::string const &phonyInputs : invocation.phonyInputs()) {
            auto it = outputToInvocation.find(phonyInputs);
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

    std::vector<Tool::Invocation const> result;
    for (Tool::Invocation const *invocation : graph.ordered()) {
        result.push_back(*invocation);
    }
    return result;
}

std::pair<bool, std::vector<Tool::Invocation const>> SimpleExecutor::
buildTarget(
    pbxproj::PBX::Target::shared_ptr const &target,
    Target::Environment const &targetEnvironment,
    std::vector<Tool::Invocation const> const &invocations)
{
    Formatter::Print(_formatter->beginWriteAuxiliaryFiles(target));
    for (Tool::Invocation const &invocation : invocations) {
        for (std::string const &output : invocation.outputs()) {
            std::string directory = FSUtil::GetDirectoryName(output);
            if (!FSUtil::TestForDirectory(directory)) {
                Formatter::Print(_formatter->createAuxiliaryDirectory(directory));

                if (!_dryRun) {
                    if (!FSUtil::CreateDirectory(FSUtil::GetDirectoryName(directory))) {
                        return std::make_pair(false, std::vector<Tool::Invocation const>());
                    }
                }
            }
        }

        for (Tool::Invocation::AuxiliaryFile const &auxiliaryFile : invocation.auxiliaryFiles()) {
            // TODO(grp): This is incorrect if the file is already written, need to check modification times.
            if (!FSUtil::TestForRead(auxiliaryFile.path())) {
                if (!FSUtil::CreateDirectory(FSUtil::GetDirectoryName(auxiliaryFile.path()))) {
                    return std::make_pair(false, std::vector<Tool::Invocation const>());
                }

                Formatter::Print(_formatter->writeAuxiliaryFile(auxiliaryFile.path()));

                if (!_dryRun) {
                    std::ofstream out;
                    out.open(auxiliaryFile.path(), std::ios::out | std::ios::trunc | std::ios::binary);
                    if (out.fail()) {
                        return std::make_pair(false, std::vector<Tool::Invocation const>());
                    }

                    out.write(auxiliaryFile.contents().data(), auxiliaryFile.contents().size() * sizeof(char));
                    out.close();
                }

                if (auxiliaryFile.executable() && !FSUtil::TestForExecute(auxiliaryFile.path())) {
                    Formatter::Print(_formatter->setAuxiliaryExecutable(auxiliaryFile.path()));

                    if (!_dryRun) {
                        if (::chmod(auxiliaryFile.path().c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
                            return std::make_pair(false, std::vector<Tool::Invocation const>());
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

    std::vector<Tool::Invocation const> orderedInvocations = SortInvocations(invocations);
    for (Tool::Invocation const &invocation : orderedInvocations) {
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

                if (!FSUtil::CreateDirectory(directory)) {
                    return std::make_pair(false, std::vector<Tool::Invocation const>({ invocation }));
                }
            }

            if (builtin) {
                std::shared_ptr<builtin::Driver> driver = _builtins.driver(executable);
                if (driver == nullptr) {
                    Formatter::Print(_formatter->finishInvocation(invocation, executable));
                    return std::make_pair(false, std::vector<Tool::Invocation const>({ invocation }));
                }

                if (driver->run(invocation.arguments(), invocation.environment(), invocation.workingDirectory()) != 0) {
                    Formatter::Print(_formatter->finishInvocation(invocation, executable));
                    return std::make_pair(false, std::vector<Tool::Invocation const>({ invocation }));
                }
            } else {
                Subprocess process;
                if (!process.execute(executable, invocation.arguments(), invocation.environment(), invocation.workingDirectory()) || process.exitcode() != 0) {
                    Formatter::Print(_formatter->finishInvocation(invocation, executable));
                    return std::make_pair(false, std::vector<Tool::Invocation const>({ invocation }));
                }
            }
        }

        Formatter::Print(_formatter->finishInvocation(invocation, executable));
    }

    return std::make_pair(true, std::vector<Tool::Invocation const>());
}

std::unique_ptr<SimpleExecutor> SimpleExecutor::
Create(std::shared_ptr<Formatter> const &formatter, bool dryRun, builtin::Registry const &builtins)
{
    return std::unique_ptr<SimpleExecutor>(new SimpleExecutor(
        formatter,
        dryRun,
        builtins
    ));
}
