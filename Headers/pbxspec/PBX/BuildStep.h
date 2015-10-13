// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_BuildStep_h
#define __pbxspec_PBX_BuildStep_h

#include <pbxspec/PBX/Specification.h>

namespace pbxspec { namespace PBX {

class BuildStep : public Specification {
public:
    typedef std::shared_ptr <BuildStep> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string _buildStepType;

protected:
    BuildStep();

public:
    virtual ~BuildStep();

public:
    inline char const *type() const override
    { return BuildStep::Type(); }

public:
    inline BuildStep::shared_ptr const &base() const
    { return reinterpret_cast <BuildStep::shared_ptr const &> (Specification::base()); }

public:
    inline std::string const &buildStepType() const
    { return _buildStepType; }

protected:
    friend class Specification;
    bool parse(Context *context, plist::Dictionary const *dict) override;

protected:
    bool inherit(Specification::shared_ptr const &base) override;
    virtual bool inherit(BuildStep::shared_ptr const &base);

protected:
    static BuildStep::shared_ptr Parse(Context *context, plist::Dictionary const *dict);

public:
    static inline char const *Type()
    { return Types::BuildStep; }
};

} }

#endif  // !__pbxspec_PBX_BuildStep_h
