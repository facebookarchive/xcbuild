// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsdk_SDK_Target_h
#define __pbxsdk_SDK_Target_h

#include <pbxsdk/SDK/Product.h>

namespace pbxsdk { namespace SDK {

class Platform;

class Target {
public:
    typedef std::shared_ptr <Target> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    friend class Platform;
    Platform            *_platform;

public:
    Product::shared_ptr  _product; 
    std::string          _path;
    std::string          _bundleName;
    std::string          _version;
    std::string          _canonicalName;
    std::string          _displayName;
    std::string          _minimalDisplayName;
    std::string          _maximumDeploymentTarget;
    string_vector        _supportedBuildToolsVersion;
    plist::Dictionary   *_customProperties;
    plist::Dictionary   *_defaultProperties;
    bool                 _isBaseSDK;

public:
    Target();
    ~Target();

public:
    inline Platform *platform() const
    { return const_cast <Target *> (this)->_platform; }

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
    inline string_vector const &supportedBuildToolsVersion() const
    { return _supportedBuildToolsVersion; }
    inline string_vector &supportedBuildToolsVersion()
    { return _supportedBuildToolsVersion; }

public:
    inline plist::Dictionary const *customProperties() const
    { return _customProperties; }

    inline plist::Dictionary const *defaultProperties() const
    { return _defaultProperties; }

public:
    inline bool isBaseSDK() const
    { return _isBaseSDK; }
    
public:
    static Target::shared_ptr Open(std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxsdk_SDK_Target_h
