// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_StaticLibraryProductType_h
#define __pbxspec_PBX_StaticLibraryProductType_h

#include <pbxspec/PBX/ProductType.h>

namespace pbxspec { namespace PBX {

class StaticLibraryProductType : public ProductType {
protected:
    friend class ProductType;
    StaticLibraryProductType(bool isDefault);

public:
    inline std::string const &isa() const
    { return ProductType::isa(); }
    bool isa(std::string const &isa) const override
    { return (ProductType::isa(isa) ||
              isa == this->isa() ||
              isa == ProductType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXStaticLibraryProductType; }
};

} }

#endif  // !__pbxspec_PBX_StaticLibraryProductType_h
