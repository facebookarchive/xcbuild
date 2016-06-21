/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <config/Config.h>
#include <plist/Format/Any.h>
#include <plist/Operations/Operations.h>

#include <memory>
#include <vector>

std::unique_ptr<plist::Object>
config::readDefaults(libutil::Filesystem const *filesystem) {
   static std::string defaultConfigPath = "/var/db/xcbuild_defaults.plist";

    std::pair<bool, std::vector<uint8_t>> result = plist::Read(filesystem, defaultConfigPath);
    if (!result.first) {
        // There isn't a defaults file.
        return nullptr;
    }

    auto format = plist::Format::Any::Identify(result.second);
    if (format == nullptr) {
        fprintf(stderr, "error: defaults file is not a plist\n");
        return nullptr;
    }

    auto deserialize = plist::Format::Any::Deserialize(result.second, *format);
    if (!deserialize.first) {
        fprintf(stderr, "error: %s\n", deserialize.second.c_str());
        return nullptr;
    }

    return std::move(deserialize.first);
}
