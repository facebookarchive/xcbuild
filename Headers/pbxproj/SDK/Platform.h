// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_SDK_Platform_h
#define __pbxproj_SDK_Platform_h

#include <pbxproj/SDK/Target.h>

namespace pbxproj { namespace SDK {

class Platform {
public:
    typedef std::shared_ptr <Platform> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    Target::vector     _targets;
    std::string        _path;
    std::string        _identifier;
    std::string        _name;
    std::string        _description;
    std::string        _type;
    std::string        _version;
    std::string        _familyIdentifier;
    std::string        _familyName;
    std::string        _icon;
    plist::Dictionary *_defaultDebuggerSettings;
    plist::Dictionary *_defaultProperties;

public:
    Platform();
    ~Platform();

public:
    inline Target::vector const &targets() const
    { return _targets; }
    inline Target::vector &targets()
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
    inline plist::Dictionary const *defaultProperties() const
    { return _defaultProperties; }

public:
    inline std::string const &icon() const
    { return _icon; }

public:
    static Platform::shared_ptr Open(std::string const &path);

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_SDK_Platform_h
