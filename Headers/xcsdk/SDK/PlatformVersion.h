// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcsdk_SDK_PlatformVersion_h
#define __xcsdk_SDK_PlatformVersion_h

#include <xcsdk/Base.h>

namespace xcsdk { namespace SDK {

class PlatformVersion {
public:
    typedef std::shared_ptr <PlatformVersion> shared_ptr;

private:
    std::string _projectName;
    std::string _productBuildVersion;
    std::string _buildVersion;
    std::string _sourceVersion;

public:
    PlatformVersion();

public:
    inline std::string const &name() const
    { return _projectName; }
    inline std::string const &version() const
    { return _productBuildVersion; }
    inline std::string const &buildVersion() const
    { return _productBuildVersion; }
    inline std::string const &sourceVersion() const
    { return _sourceVersion; }

public:
    static PlatformVersion::shared_ptr Open(std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_PlatformVersion_h
