// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_ToolSpecificationHeadermapGenerator_h
#define __pbxspec_XC_ToolSpecificationHeadermapGenerator_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace XC {

class ToolSpecificationHeadermapGenerator : public PBX::Tool {
protected:
    friend class Tool;
    ToolSpecificationHeadermapGenerator();

public:
    inline std::string const &isa() const
    { return PBX::Tool::isa(); }
    bool isa(std::string const &isa) const override
    { return (PBX::Tool::isa(isa) ||
              isa == this->isa() ||
              isa == PBX::Tool::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCToolSpecificationHeadermapGenerator; }
};

} }

#endif  // !__pbxspec_XC_ToolSpecificationHeadermapGenerator_h
