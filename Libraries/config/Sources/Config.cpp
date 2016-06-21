/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <config/Config.h>
#include <plist/plist.h>

#include <memory>
#include <vector>

using config::Config;

template<typename T>
T _loadValue(plist::Object *config, std::string const &configKey);

std::vector<std::string> _loadValue(plist::Object *config, std::string const &configKey)
{
    std::vector<std::string> values;
    plist::Dictionary *dictionary;
    plist::Object *configValue;
    plist::Array *array;
    if (config != nullptr &&
        (dictionary = plist::CastTo<plist::Dictionary>(config)) &&
        (configValue = dictionary->value(configKey)) &&
        (array = plist::CastTo<plist::Array>(configValue))) {
        for (auto iter = array->begin(); iter != array->end(); iter++) {
            auto value = plist::CastTo<plist::String>(iter->get());

            if (value != nullptr) {
                values.push_back(value->value());
            }
        }
    }

    return values;
}

const std::string
Config::DefaultPath()
{
    if (char const *environConfigPath = getenv("CONFIG_PATH")) {
        return std::string(environConfigPath);
    } else {
        return "/var/db/xcbuild_defaults.plist";
    }
}

Config Config::
Open(libutil::Filesystem const *filesystem, std::string const &defaultPath)
{
    // TODO return an ext::optional<Config>
    auto plist = plist::Read(filesystem, defaultPath);
    return Config(
        _loadValue(plist.get(), "ExtraPlatformPaths"),
        _loadValue(plist.get(), "ExtraToolchainPaths")
    );
}
