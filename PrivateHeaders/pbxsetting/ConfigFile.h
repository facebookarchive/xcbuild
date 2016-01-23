/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxsetting_ConfigFile_h
#define __pbxsetting_ConfigFile_h

#include <pbxsetting/XC/Config.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Environment.h>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>

namespace pbxsetting {

class ConfigFile {
private:
    struct File {
        std::FILE   *fp;
        std::string  path;
    };

private:
    std::unordered_set<std::string> _included;
    std::vector<File>               _files;
    File                            _current;
    std::stringstream               _processed;
    bool                            _stop;
    XC::Config::error_function      _error;

public:
    ConfigFile();

public:
    std::pair<bool, Level>
    open(std::string const &path, Environment const &environment, XC::Config::error_function const &error);

private:
    bool parse(std::string const &path, Environment const &environment);

private:
    void push();
    void pop();
    void process(Environment const &environment);

private:
    void error(unsigned line, std::string format, ...);
};

}

#endif  // !__pbxsetting_ConfigFile_h
