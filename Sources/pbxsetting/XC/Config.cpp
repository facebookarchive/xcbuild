// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/XC/Config.h>
#include <pbxsetting/ConfigFile.h>

using pbxsetting::XC::Config;

Config::Config() :
    _settings(nullptr)
{
}

Config::~Config()
{
    if (_settings != nullptr) {
        _settings->release();
    }
}

Config::shared_ptr Config::
Open(std::string const &path, error_function const &error)
{
    plist::Dictionary *settings = ConfigFile().open(path, error);
    if (settings == nullptr)
        return nullptr;

    auto config = std::make_shared <Config> ();
    config->_settings = settings;

    return config;
}

Config::shared_ptr Config::
Open(std::string const &path)
{
    return Open(path,
            [](std::string const &, unsigned, std::string const &)
            {
                return true;
            });
}
