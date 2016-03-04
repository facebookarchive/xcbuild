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

/*
 * Represents the contents of a developer root, containing toolchains,
 * platforms, and SDKs. There is usually only one developer root.
 */
class Manager {
private:
    std::string       _path;
    Platform::vector  _platforms;
    Toolchain::vector _toolchains;

public:
    Manager();
    ~Manager();

public:
    /*
     * Platforms included in the developer root.
     */
    inline Platform::vector const &platforms() const
    { return _platforms; }

    /*
     * Toolchains included in the developer root.
     */
    inline Toolchain::vector const &toolchains() const
    { return _toolchains; }

public:
    /*
     * The path to the developer root.
     */
    inline std::string const &path() const
    { return _path; }

public:
    /*
     * Find an SDK by name. This does a fuzzy search, so the "name" could
     * be an SDK name or path, or even the name of a platform.
     */
    Target::shared_ptr findTarget(std::string const &name) const;

    /*
     * Find a toolchain by name. This does a fuzzy search; the "name" could
     * be a name, path, or identifier for the toolchain.
     */
    Toolchain::shared_ptr findToolchain(std::string const &name) const;

public:
    /*
     * Default settings for the contents of the developer root.
     */
    pbxsetting::Level computedSettings(void) const;

public:
    /*
     * Standard executable paths for tools found directly in the developer
     * root, rather than within a toolchain or a platform.
     */
    std::vector<std::string> executablePaths() const;

public:
    /*
     * Load from a developer root. Returns nullptr on error.
     */
    static std::shared_ptr<Manager> Open(std::string const &path);
};

} }

#endif  // !__xcsdk_SDK_Manager_h
