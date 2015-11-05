/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcsdk_SDK_Manager_h
#define __xcsdk_SDK_Manager_h

#include <xcsdk/SDK/Platform.h>
#include <xcsdk/SDK/Toolchain.h>
#include <xcsdk/SDK/Target.h>

namespace xcsdk { namespace SDK {

class Manager {
private:
    std::string _path;
    Platform::vector _platforms;
    Toolchain::map _toolchains;

public:
    Manager();
    ~Manager();

public:
    inline Platform::vector const &platforms() const
    { return _platforms; }
    inline Toolchain::map const &toolchains() const
    { return _toolchains; }

public:
    inline std::string const &path() const
    { return _path; }

public:
    Target::shared_ptr findTarget(std::string const &name) const;

public:
    pbxsetting::Level computedSettings(void) const;

public:
    std::vector<std::string> executablePaths() const;

public:
    static std::shared_ptr<Manager> Open(std::string const &path);
};

} }

#endif  // !__xcsdk_SDK_Manager_h
