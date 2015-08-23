// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_Target_h
#define __pbxproj_PBX_Target_h

#include <pbxproj/XC/ConfigurationList.h>
#include <pbxproj/PBX/BuildRule.h>
#include <pbxproj/PBX/BuildPhase.h>
#include <pbxproj/PBX/TargetDependency.h>

namespace pbxproj { namespace PBX {

class Target : public Object {
public:
    typedef std::shared_ptr <Target> shared_ptr;
    typedef std::vector <shared_ptr> vector;
    typedef std::map <std::string, shared_ptr> map;

public:
    enum Type {
        kTypeNative,
        kTypeLegacy
    };

private:
    Type                              _type;

private:
    friend class Project;
    Project                          *_project;

private:
    std::string                       _name;
    std::string                       _productName;
    XC::ConfigurationList::shared_ptr _buildConfigurationList;
    PBX::BuildPhase::vector           _buildPhases;
    PBX::TargetDependency::vector     _dependencies;

protected:
    Target(std::string const &isa, Type type);

public:
    inline Type type() const
    { return _type; }

public:
    inline Project const *project() const
    { return _project; }

public:
    inline std::string const &name() const
    { return _name; }

public:
    inline std::string const &productName() const
    { return _productName; }

public:
    inline XC::ConfigurationList::shared_ptr const &buildConfigurationList() const
    { return _buildConfigurationList; }
    inline XC::ConfigurationList::shared_ptr &buildConfigurationList()
    { return _buildConfigurationList; }

public:
    inline BuildPhase::vector const &buildPhases() const
    { return _buildPhases; }
    inline BuildPhase::vector &buildPhases()
    { return _buildPhases; }

public:
    inline TargetDependency::vector const &dependencies() const
    { return _dependencies; }
    inline TargetDependency::vector &dependencies()
    { return _dependencies; }

public:
    pbxsetting::Level settings(void) const;

public:
    bool parse(Context &context, plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_PBX_Target_h
