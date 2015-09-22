// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_StaticFrameworkProductType_h
#define __pbxspec_XC_StaticFrameworkProductType_h

#include <pbxspec/PBX/ProductType.h>

namespace pbxspec { namespace XC {

class StaticFrameworkProductType : public PBX::ProductType {
protected:
    friend class ProductType;
    StaticFrameworkProductType();

public:
    inline std::string const &isa() const
    { return ProductType::isa(); }
    bool isa(std::string const &isa) const override
    { return (ProductType::isa(isa) ||
              isa == this->isa() ||
              isa == ProductType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCStaticFrameworkProductType; }
};

} }

#endif  // !__pbxspec_XC_StaticFrameworkProductType_h
