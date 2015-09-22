// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_FrameworkProductType_h
#define __pbxspec_PBX_FrameworkProductType_h

#include <pbxspec/PBX/ProductType.h>

namespace pbxspec { namespace PBX {

class FrameworkProductType : public ProductType {
protected:
    friend class ProductType;
    FrameworkProductType();

public:
    inline std::string const &isa() const
    { return ProductType::isa(); }
    bool isa(std::string const &isa) const override
    { return (ProductType::isa(isa) ||
              isa == this->isa() ||
              isa == ProductType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXFrameworkProductType; }
};

} }

#endif  // !__pbxspec_PBX_FrameworkProductType_h
