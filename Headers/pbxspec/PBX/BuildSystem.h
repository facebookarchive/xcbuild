// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_BuildSystem_h
#define __pbxspec_PBX_BuildSystem_h

#include <pbxspec/PBX/Specification.h>
#include <pbxspec/PBX/PropertyOption.h>

namespace pbxspec { namespace PBX {

class BuildSystem : public Specification {
public:
    typedef std::shared_ptr <BuildSystem> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    bool                   _isGlobalDomainInUI;
    PropertyOption::vector _options;
    PropertyOption::vector _properties;

protected:
    BuildSystem(bool isDefault);

public:
    virtual ~BuildSystem();

public:
    inline char const *type() const override
    { return BuildSystem::Type(); }

public:
    inline BuildSystem::shared_ptr const &base() const
    { return reinterpret_cast <BuildSystem::shared_ptr const &> (Specification::base()); }

public:
    inline bool isGlobalDomainInUI() const
    { return _isGlobalDomainInUI; }

public:
    inline PropertyOption::vector const &options() const
    { return _options; }

public:
    inline PropertyOption::vector const &properties() const
    { return _properties; }

public:
    pbxsetting::Level defaultSettings(void) const;

protected:
    friend class Specification;
    bool parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(BuildSystem::shared_ptr const &base);

protected:
    static BuildSystem::shared_ptr Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXBuildSystem; }
    static inline char const *Type()
    { return Types::BuildSystem; }
};

} }

#endif  // !__pbxspec_PBX_BuildSystem_h
