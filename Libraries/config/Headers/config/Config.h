/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __config_config_h
#define __config_config_h

#include <libutil/Filesystem.h>
#include <plist/Object.h>

namespace config {

class Config {
public:
    Config(std::vector<std::string> extraPlatformPaths, std::vector<std::string> extraToolchainPaths) :
        _extraPlatformPaths(extraPlatformPaths),
        _extraToolchainPaths(extraToolchainPaths) {}

    Config() {}

private:
    std::vector<std::string> _extraPlatformPaths;
    std::vector<std::string> _extraToolchainPaths;

public:
    inline const std::vector<std::string>& extraPlatformPaths()  const
    { return _extraPlatformPaths; }

    inline const std::vector<std::string>& extraToolchainPaths() const
    { return _extraToolchainPaths; }

public:
    static const std::string DefaultPath();
    static Config Open(libutil::Filesystem const *filesystem, std::string const &defaultPath);
};

}

#endif  // !__config_config_h
