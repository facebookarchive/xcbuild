// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ToolInvocation_h
#define __pbxbuild_ToolInvocation_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class ToolInvocation {
public:
    class AuxiliaryFile {
    private:
        std::string _path;
        std::string _contents;
        bool        _executable;

    public:
        AuxiliaryFile(std::string const &path, std::string const &contents, bool executable);
        ~AuxiliaryFile();

    public:
        std::string const &path() const
        { return _path; }
        std::string const &contents() const
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
        std::string const                                  &logMessage
    );
    ~ToolInvocation();

public:
    std::string const &executable(void) const
    { return _executable; }
    std::vector<std::string> const &arguments(void) const
    { return _arguments; }
    std::unordered_map<std::string, std::string> const &environment(void) const
    { return _environment; }
    std::string const &workingDirectory(void) const
    { return _workingDirectory; }

public:
    std::vector<std::string> const &inputs(void) const
    { return _inputs; }
    std::vector<std::string> const &outputs(void) const
    { return _outputs; }
    std::string const &dependencyInfo(void) const
    { return _dependencyInfo; }
    std::vector<AuxiliaryFile> const &auxiliaryFiles(void) const
    { return _auxiliaryFiles; }

public:
    std::string const &logMessage(void) const
    { return _logMessage; }
};

}

#endif // !__pbxbuild_ToolInvocation_h
