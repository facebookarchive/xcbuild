/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_ToolInvocation_h
#define __pbxbuild_ToolInvocation_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class ToolInvocation {
public:
    class AuxiliaryFile {
    private:
        std::string        _path;
        std::vector<char> _contents;
        bool              _executable;

    public:
        AuxiliaryFile(std::string const &path, std::vector<char> const &contents, bool executable);
        AuxiliaryFile(std::string const &path, std::string const &contents, bool executable);
        ~AuxiliaryFile();

    public:
        std::string const &path() const
        { return _path; }
        std::vector<char> const &contents() const
        { return _contents; }
        bool executable() const
        { return _executable; }
    };

private:
    std::string                                  _executable;
    std::vector<std::string>                     _arguments;
    std::unordered_map<std::string, std::string> _environment;
    std::string                                  _workingDirectory;

private:
    std::vector<std::string>                     _inputs;
    std::vector<std::string>                     _outputs;
    std::string                                  _dependencyInfo;
    std::vector<AuxiliaryFile>                   _auxiliaryFiles;

private:
    std::string                                  _logMessage;
    bool                                         _showEnvironmentInLog;

public:
    ToolInvocation(
        std::string const                                  &executable,
        std::vector<std::string> const                     &arguments,
        std::unordered_map<std::string, std::string> const &environment,
        std::string const                                  &workingDirectory,
        std::vector<std::string> const                     &inputs,
        std::vector<std::string> const                     &outputs,
        std::string const                                  &dependencyInfo,
        std::vector<AuxiliaryFile> const                   &auxiliaryFiles,
        std::string const                                  &logMessage,
        bool                                               showEnvironmentInLog = true
    );
    ToolInvocation(
        std::vector<std::string> const                     &inputs,
        std::vector<std::string> const                     &outputs,
        std::vector<AuxiliaryFile> const                   &auxiliaryFiles
    );
    ~ToolInvocation();

public:
    std::string const &executable() const
    { return _executable; }
    std::vector<std::string> const &arguments() const
    { return _arguments; }
    std::unordered_map<std::string, std::string> const &environment() const
    { return _environment; }
    std::string const &workingDirectory() const
    { return _workingDirectory; }

public:
    std::vector<std::string> const &inputs() const
    { return _inputs; }
    std::vector<std::string> const &outputs() const
    { return _outputs; }
    std::string const &dependencyInfo() const
    { return _dependencyInfo; }
    std::vector<AuxiliaryFile> const &auxiliaryFiles() const
    { return _auxiliaryFiles; }

public:
    std::string const &logMessage() const
    { return _logMessage; }
    bool showEnvironmentInLog() const
    { return _showEnvironmentInLog; }
};

}

#endif // !__pbxbuild_ToolInvocation_h
