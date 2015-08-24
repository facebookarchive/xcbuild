// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_AggregateTarget_h
#define __pbxproj_PBX_AggregateTarget_h

#include <pbxproj/PBX/Target.h>

namespace pbxproj { namespace PBX {

class AggregateTarget : public Target {
public:
    typedef std::shared_ptr <AggregateTarget> shared_ptr;

private:
    std::string  _productName;

public:
    AggregateTarget();

public:
    inline std::string const &productName() const
    { return _productName; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXAggregateTarget; }
};

} }

#endif  // !__pbxproj_PBX_AggregateTarget_h
