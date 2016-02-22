/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxsetting_XC_Config_h
#define __pbxsetting_XC_Config_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Environment.h>

#include <memory>
#include <string>
#include <vector>

namespace pbxsetting { namespace XC {

class Config {
public:
    typedef std::shared_ptr <Config> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string _path;
    Level       _level;

public:
    typedef std::function <bool(std::string const &filename, unsigned line,
            std::string const &message)> error_function;

public:
    Config();
    ~Config();

public:
    inline std::string const &path() const
    { return _path; }
    inline Level const &level() const
    { return _level; }

public:
    static Config::shared_ptr
    Open(std::string const &path, Environment const &environment);
    static Config::shared_ptr
    Open(std::string const &path, Environment const &environment, error_function const &error);
};

} }

#endif  // !__pbxsetting_XC_Config_h
