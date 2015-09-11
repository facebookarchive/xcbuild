// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/BuildContext.h>

using pbxbuild::ToolInvocation;
using AuxiliaryFile = pbxbuild::ToolInvocation::AuxiliaryFile;

AuxiliaryFile::
AuxiliaryFile(std::string const &path, std::vector<char> const &contents, bool executable) :
    _path      (path),
    _contents  (contents),
    _executable(executable)
{
}

AuxiliaryFile::
AuxiliaryFile(std::string const &path, std::string const &contents, bool executable) :
    _path      (path),
    _contents  (std::vector<char>(contents.begin(), contents.end())),
    _executable(executable)
{
}

AuxiliaryFile::
~AuxiliaryFile()
{
}

ToolInvocation::
ToolInvocation(
    std::string const                                  &executable,
    std::vector<std::string> const                     &arguments,
    std::unordered_map<std::string, std::string> const &environment,
    std::string const                                  &workingDirectory,
    std::vector<std::string> const                     &inputs,
    std::vector<std::string> const                     &outputs,
    std::string const                                  &dependencyInfo,
    std::vector<AuxiliaryFile> const                   &auxiliaryFiles,
    std::string const                                  &logMessage
) :
    _executable      (executable),
    _arguments       (arguments),
    _environment     (environment),
    _workingDirectory(workingDirectory),
    _inputs          (inputs),
    _outputs         (outputs),
    _dependencyInfo  (dependencyInfo),
    _auxiliaryFiles  (auxiliaryFiles),
    _logMessage      (logMessage)
{
}

ToolInvocation::
ToolInvocation(
    std::vector<std::string> const                     &inputs,
    std::vector<std::string> const                     &outputs,
    std::vector<AuxiliaryFile> const                   &auxiliaryFiles
) :
    _executable    (""),
    _inputs        (inputs),
    _outputs       (outputs),
    _auxiliaryFiles(auxiliaryFiles)
{
}

ToolInvocation::
~ToolInvocation()
{
}

