// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/BuildContext.h>

using pbxbuild::ToolInvocation;

ToolInvocation::
ToolInvocation(
    std::string const                        &executable,
    std::vector<std::string> const           &arguments,
    std::map<std::string, std::string> const &environment,
    std::string const                        &workingDirectory,
    std::vector<std::string> const           &inputs,
    std::vector<std::string> const           &outputs,
    std::string const                        &dependencyInfo,
    std::string const                        &logMessage
) :
    _executable      (executable),
    _arguments       (arguments),
    _environment     (environment),
    _workingDirectory(workingDirectory),
    _inputs          (inputs),
    _outputs         (outputs),
    _dependencyInfo  (dependencyInfo),
    _logMessage      (logMessage)
{
}

ToolInvocation::
~ToolInvocation()
{
}

