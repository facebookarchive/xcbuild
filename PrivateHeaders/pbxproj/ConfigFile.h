// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_ConfigFile_h
#define __pbxproj_ConfigFile_h

#include <pbxproj/XC/Config.h>
#include <sstream>

namespace pbxproj {

class ConfigFile {
private:
    struct File {
        std::FILE   *fp;
        std::string  path;
    };

private:
    string_set                 _included;
    std::vector <File>         _files;
    File                       _current;
    std::stringstream          _processed;
    bool                       _stop;
    XC::Config::error_function _error;

public:
    ConfigFile();

public:
    plist::Dictionary *open(std::string const &path,
            XC::Config::error_function const &error);

private:
    bool parse(std::string const &path);

private:
    void push();
    void pop();
    void process();

private:
    void error(unsigned line, std::string const &format, ...);
};

}

#endif  // !__pbxproj_ConfigFile_h
