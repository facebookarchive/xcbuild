// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_ApplicationProductType_h
#define __pbxspec_PBX_ApplicationProductType_h

#include <pbxspec/PBX/ProductType.h>

namespace pbxspec { namespace PBX {

class ApplicationProductType : public ProductType {
protected:
    friend class ProductType;
    ApplicationProductType();

public:
    inline std::string const &isa() const
    { return ProductType::isa(); }
    bool isa(std::string const &isa) const override
    { return (ProductType::isa(isa) ||
              isa == this->isa() ||
              isa == ProductType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXApplicationProductType; }
};

} }

#endif  // !__pbxspec_PBX_ApplicationProductType_h
