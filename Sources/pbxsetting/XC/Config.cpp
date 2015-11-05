/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
