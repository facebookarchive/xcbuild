// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_BuildConfiguration_h
#define __pbxproj_XC_BuildConfiguration_h

#include <pbxproj/PBX/FileReference.h>

namespace pbxproj { namespace XC {

class BuildConfiguration : public PBX::Object {
public:
    typedef std::shared_ptr <BuildConfiguration> shared_ptr;
    typedef std::vector <shared_ptr> vector;
    typedef std::map <std::string, shared_ptr> map;

private:
    std::string                     _name;
    PBX::FileReference::shared_ptr  _baseConfigurationReference;
    plist::Object                  *_buildSettings;

public:
    BuildConfiguration();
    ~BuildConfiguration();

public:
    inline PBX::FileReference::shared_ptr const &baseConfigurationReference() const
    { return _baseConfigurationReference; }
    inline PBX::FileReference::shared_ptr &baseConfigurationReference()
    { return _baseConfigurationReference; }

public:
    inline plist::Object const *buildSettings() const
    { return _buildSettings; }

public:
    inline std::string const &name() const
    { return _name; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::XCBuildConfiguration; }
};

} }

#endif  // !__pbxproj_XC_BuildConfiguration_h
