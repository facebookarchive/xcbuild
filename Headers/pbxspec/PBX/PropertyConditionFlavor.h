// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_PropertyConditionFlavor_h
#define __pbxspec_PBX_PropertyConditionFlavor_h

#include <pbxspec/PBX/Specification.h>

namespace pbxspec { namespace PBX {

class PropertyConditionFlavor : public Specification {
public:
    typedef std::shared_ptr <PropertyConditionFlavor> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    int _precedence;

protected:
    PropertyConditionFlavor(bool isDefault);

public:
    virtual ~PropertyConditionFlavor();

public:
    inline char const *type() const override
    { return PropertyConditionFlavor::Type(); }

public:
    inline PropertyConditionFlavor::shared_ptr const &base() const
    { return reinterpret_cast <PropertyConditionFlavor::shared_ptr const &> (Specification::base()); }

public:
    inline int precedence() const
    { return _precedence; }

protected:
    friend class Specification;
    bool parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(PropertyConditionFlavor::shared_ptr const &base);

protected:
    static PropertyConditionFlavor::shared_ptr Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXPropertyConditionFlavor; }
    static inline char const *Type()
    { return Types::PropertyConditionFlavor; }
};

} }

#endif  // !__pbxspec_PBX_PropertyConditionFlavor_h
