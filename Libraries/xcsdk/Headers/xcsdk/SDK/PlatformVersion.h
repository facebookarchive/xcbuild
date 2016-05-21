/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcsdk_SDK_PlatformVersion_h
#define __xcsdk_SDK_PlatformVersion_h

#include <xcsdk/Base.h>

namespace libutil { class Filesystem; };

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
    static PlatformVersion::shared_ptr Open(libutil::Filesystem const *filesystem, std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_PlatformVersion_h
