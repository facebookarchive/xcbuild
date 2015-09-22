// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_BuildSettings_h
#define __pbxspec_PBX_BuildSettings_h

#include <pbxspec/PBX/Specification.h>
#include <pbxspec/PBX/PropertyOption.h>

namespace pbxspec { namespace PBX {

class BuildSettings : public Specification {
public:
    typedef std::shared_ptr <BuildSettings> shared_ptr;
    typedef std::vector <shared_ptr> vector;

protected:
    PropertyOption::vector   _options;
    PropertyOption::used_map _optionsUsed;

protected:
    BuildSettings();

public:
    virtual ~BuildSettings();

public:
    inline char const *type() const override
    { return BuildSettings::Type(); }

public:
    inline BuildSettings::shared_ptr const &base() const
    { return reinterpret_cast <BuildSettings::shared_ptr const &> (Specification::base()); }

public:
    inline PropertyOption::vector const &options() const
    { return _options; }

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(BuildSettings::shared_ptr const &base);

protected:
    static BuildSettings::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::BuildSettings; }
};

} }

#endif  // !__pbxspec_PBX_BuildSettings_h
