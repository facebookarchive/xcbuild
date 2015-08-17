// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/XC/Config.h>
#include <pbxsetting/ConfigFile.h>

using pbxsetting::XC::Config;

Config::Config() :
    _level(Level({ }))
{
}

Config::~Config()
{
}

Config::shared_ptr Config::
Open(std::string const &path, Environment const &environment, error_function const &error)
{
    std::pair<bool, Level> result = ConfigFile().open(path, environment, error);
    if (!result.first) {
        return nullptr;
    }

    auto config = std::make_shared <Config> ();
    config->_level = result.second;

    return config;
}

Config::shared_ptr Config::
Open(std::string const &path, Environment const &environment)
{
    return Open(path, environment,
            [](std::string const &, unsigned, std::string const &)
            {
                return true;
            });
}
