/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/CopyInvocationContext.h>
#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::CopyInvocationContext;
using pbxbuild::Tool::ToolInvocationContext;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::ToolInvocation;
using libutil::FSUtil;

CopyInvocationContext::
CopyInvocationContext(ToolInvocation const &invocation) :
    _invocation(invocation)
{
}

CopyInvocationContext::
~CopyInvocationContext()
{
}

CopyInvocationContext CopyInvocationContext::
Create(
    pbxspec::PBX::Tool::shared_ptr const &copyTool,
    std::string const &inputFile,
    std::string const &outputDirectory,
    std::string const &logMessageTitle,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    std::string outputFile = outputDirectory + "/" + FSUtil::GetBaseName(inputFile);

    std::string logMessage = logMessageTitle + " " + FSUtil::GetRelativePath(inputFile, workingDirectory) + " " + outputFile;

    ToolEnvironment toolEnvironment = ToolEnvironment::Create(copyTool, environment, { inputFile }, { outputFile });
    OptionsResult options = OptionsResult::Create(toolEnvironment, workingDirectory, nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "", { inputFile, outputDirectory });
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory);
    return CopyInvocationContext(context.invocation());
}
