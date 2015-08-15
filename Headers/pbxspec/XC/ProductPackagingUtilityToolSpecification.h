// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_ProductPackagingUtilityToolSpecification_h
#define __pbxspec_XC_ProductPackagingUtilityToolSpecification_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace XC {

class ProductPackagingUtilityToolSpecification : public PBX::Tool {
protected:
    friend class Tool;
    ProductPackagingUtilityToolSpecification(bool isDefault);

public:
    inline std::string const &isa() const
    { return PBX::Tool::isa(); }
    bool isa(std::string const &isa) const override
    { return (PBX::Tool::isa(isa) ||
              isa == this->isa() ||
              isa == PBX::Tool::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCProductPackagingUtilityToolSpecification; }
};

} }

#endif  // !__pbxspec_XC_ProductPackagingUtilityToolSpecification_h
