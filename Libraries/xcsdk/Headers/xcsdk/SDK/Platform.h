/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcsdk_SDK_Platform_h
#define __xcsdk_SDK_Platform_h

#include <xcsdk/SDK/Target.h>
#include <xcsdk/SDK/PlatformVersion.h>
#include <pbxsetting/Level.h>

#include <memory>
#include <string>
#include <vector>

namespace libutil { class Filesystem; };
namespace plist { class Dictionary; }

namespace xcsdk { namespace SDK {

class Platform {
public:
    typedef std::shared_ptr <Platform> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::weak_ptr<Manager>          _manager;
    PlatformVersion::shared_ptr     _platformVersion;
    std::vector<Target::shared_ptr> _targets;
    std::string                     _path;
    std::string                     _identifier;
    std::string                     _name;
    std::string                     _description;
    std::string                     _type;
    std::string                     _version;
    std::string                     _familyIdentifier;
    std::string                     _familyName;
    std::string                     _icon;
    plist::Dictionary              *_defaultDebuggerSettings;
    pbxsetting::Level               _defaultProperties;
    pbxsetting::Level               _overrideProperties;

public:
    Platform();
    ~Platform();

public:
    inline std::shared_ptr<Manager> manager() const
    { return _manager.lock(); }

public:
    inline PlatformVersion::shared_ptr const &platformVersion() const
    { return _platformVersion; }
    inline std::vector<Target::shared_ptr> const &targets() const
    { return _targets; }

public:
    inline std::string const &path() const
    { return _path; }

public:
    inline std::string const &identifier() const
    { return _identifier; }
    inline std::string const &name() const
    { return _name; }
    inline std::string const &description() const
    { return _description; }
    inline std::string const &version() const
    { return _version; }
    inline std::string const &type() const
    { return _type; }

public:
    inline std::string const familyIdentifier() const
    { return _familyIdentifier; }
    inline std::string const familyName() const
    { return _familyName; }

public:
    inline plist::Dictionary const *defaultDebuggerSettings() const
    { return _defaultDebuggerSettings; }
    inline pbxsetting::Level const &defaultProperties() const
    { return _defaultProperties; }
    inline pbxsetting::Level const &overrideProperties() const
    { return _overrideProperties; }

public:
    inline std::string const &icon() const
    { return _icon; }

public:
    pbxsetting::Level settings() const;

public:
    std::vector<std::string> executablePaths() const;

public:
    static Platform::shared_ptr Open(libutil::Filesystem const *filesystem, std::shared_ptr<Manager> manager, std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcsdk_SDK_Platform_h
