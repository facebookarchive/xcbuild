// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_XC_Config_h
#define __pbxsetting_XC_Config_h

#include <plist/plist.h>
#include <pbxsetting/Base.h>

namespace pbxsetting { namespace XC {

class Config {
public:
    typedef std::shared_ptr <Config> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    plist::Dictionary *_settings;

public:
    typedef std::function <bool(std::string const &filename, unsigned line,
            std::string const &message)> error_function;

public:
    Config();
    ~Config();

public:
    inline plist::Dictionary const *settings() const
    { return _settings; }
    inline plist::Dictionary *settings()
    { return _settings; }

public:
    static Config::shared_ptr Open(std::string const &path);
    static Config::shared_ptr Open(std::string const &path,
            error_function const &error);
};

} }

#endif  // !__pbxsetting_XC_Config_h
