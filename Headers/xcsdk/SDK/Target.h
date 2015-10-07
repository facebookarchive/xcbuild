// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcsdk_SDK_Target_h
#define __xcsdk_SDK_Target_h

#include <xcsdk/SDK/Product.h>
#include <xcsdk/SDK/Toolchain.h>

namespace xcsdk { namespace SDK {

class Manager;
class Platform;

class Target {
public:
    typedef std::shared_ptr <Target> shared_ptr;
    typedef std::weak_ptr <Target> weak_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    std::weak_ptr<Manager>  _manager;
    std::weak_ptr<Platform> _platform;

public:
    Product::shared_ptr      _product;
    std::string              _path;
    std::string              _bundleName;
    std::string              _version;
    std::string               _canonicalName;
    std::string              _displayName;
    std::string              _minimalDisplayName;
    std::string              _maximumDeploymentTarget;
    std::vector<std::string> _supportedBuildToolsVersion;
    pbxsetting::Level        _customProperties;
    pbxsetting::Level        _defaultProperties;
    bool                     _isBaseSDK;
    Toolchain::vector        _toolchains;

public:
    Target();
    ~Target();

public:
    inline std::shared_ptr<Manager> manager() const
    { return _manager.lock(); }
    inline std::shared_ptr<Platform> platform() const
    { return _platform.lock(); }

    inline std::string const &path() const
    { return _path; }
    inline std::string const &bundleName() const
    { return _bundleName; }

public:
    inline Product::shared_ptr const &product() const
    { return _product; }

public:
    inline std::string const &version() const
    { return _version; }

public:
    inline std::string const &canonicalName() const
    { return _canonicalName; }

public:
    inline std::string const &displayName() const
    { return _displayName; }

    inline std::string const &minimalDisplayName() const
    { return _minimalDisplayName; }

public:
    inline std::string const &maximumDeploymentTarget() const
    { return _maximumDeploymentTarget; }

public:
    inline std::vector<std::string> const &supportedBuildToolsVersion() const
    { return _supportedBuildToolsVersion; }
    inline std::vector<std::string> &supportedBuildToolsVersion()
    { return _supportedBuildToolsVersion; }

public:
    inline pbxsetting::Level const &customProperties() const
    { return _customProperties; }

    inline pbxsetting::Level const &defaultProperties() const
    { return _defaultProperties; }

public:
    inline bool isBaseSDK() const
    { return _isBaseSDK; }

public:
    inline Toolchain::vector const &toolchains() const
    { return _toolchains; }

public:
    pbxsetting::Level settings(void) const;

public:
    std::vector<std::string> executablePaths() const;

public:
    static Target::shared_ptr Open(std::shared_ptr<Manager> manager, std::shared_ptr<Platform>, std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_Target_h
