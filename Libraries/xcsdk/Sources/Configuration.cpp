/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/Configuration.h>
#include <libutil/Filesystem.h>
#include <plist/Array.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <plist/Format/Any.h>

using xcsdk::Configuration;
using libutil::Filesystem;

Configuration::
Configuration(
    std::vector<std::string> const &extraPlatformsPaths,
    std::vector<std::string> const &extraToolchainsPaths) :
    _extraPlatformsPaths (extraPlatformsPaths),
    _extraToolchainsPaths(extraToolchainsPaths)
{
}

std::string Configuration::
DefaultPath()
{
    if (char const *environmentPath = getenv("XCSDK_CONFIGURATION_PATH")) {
        return std::string(environmentPath);
    } else {
        return "/var/db/xcsdk_configuration.plist";
    }
}

static std::vector<std::string>
LoadArray(plist::Array const *array)
{
    std::vector<std::string> values;

    if (array != nullptr) {
        for (size_t n = 0; n < array->count(); n++) {
            if (auto value = array->value<plist::String>(n)) {
                values.push_back(value->value());
            }
        }
    }

    return values;
}

ext::optional<Configuration> Configuration::
Load(Filesystem const *filesystem, std::string const &path)
{
    std::vector<uint8_t> contents;
    if (!filesystem->read(&contents, path)) {
        return ext::nullopt;
    }

    auto result = plist::Format::Any::Deserialize(contents);
    if (result.first == nullptr) {
        return ext::nullopt;
    }

    plist::Dictionary const *root = plist::CastTo<plist::Dictionary>(result.first.get());
    if (root == nullptr) {
        return ext::nullopt;
    }

    std::vector<std::string> platforms = LoadArray(root->value<plist::Array>("ExtraPlatformsPaths"));
    std::vector<std::string> toolchains = LoadArray(root->value<plist::Array>("ExtraToolchainsPaths"));
    return Configuration(platforms, toolchains);
}
