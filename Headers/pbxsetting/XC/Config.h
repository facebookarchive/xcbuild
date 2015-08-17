// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_XC_Config_h
#define __pbxsetting_XC_Config_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Environment.h>

namespace pbxsetting { namespace XC {

class Config {
public:
    typedef std::shared_ptr <Config> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    Level _level;

public:
    typedef std::function <bool(std::string const &filename, unsigned line,
            std::string const &message)> error_function;

public:
    Config();
    ~Config();

public:
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
