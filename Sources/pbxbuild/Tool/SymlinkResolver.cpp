/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/SymlinkResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/TypeResolvedFile.h>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::SymlinkResolver::
SymlinkResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

void Tool::SymlinkResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &symlinkPath,
    std::string const &targetPath,
    bool productStructure) const
{
    std::string logMessage = "SymLink " + targetPath + " " + symlinkPath;

    Tool::Environment toolEnvironment = Tool::Environment::Create(_tool, environment, { targetPath }, { symlinkPath });
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, workingDirectory, nullptr);
    Tool::CommandLineResult commandLine = Tool::CommandLineResult::Create(toolEnvironment, options, "/bin/ln", { "-sfh", targetPath, symlinkPath });

    Tool::Invocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = workingDirectory;
    invocation.phonyInputs() = toolEnvironment.inputs(workingDirectory);
    invocation.outputs() = toolEnvironment.outputs(workingDirectory);
    invocation.logMessage() = logMessage;
    invocation.createsProductStructure() = productStructure;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::SymlinkResolver> Tool::SymlinkResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr symlinkTool = buildEnvironment.specManager()->tool(Tool::SymlinkResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (symlinkTool == nullptr) {
        fprintf(stderr, "warning: could not find symlink tool\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::SymlinkResolver>(new Tool::SymlinkResolver(symlinkTool));
}

