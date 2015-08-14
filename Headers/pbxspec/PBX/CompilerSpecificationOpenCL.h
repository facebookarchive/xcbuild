// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_CompilerSpecificationOpenCL_h
#define __pbxspec_PBX_CompilerSpecificationOpenCL_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace PBX {

class CompilerSpecificationOpenCL : public Tool {
protected:
    friend class Tool;
    CompilerSpecificationOpenCL(bool isDefault);

public:
    inline std::string const &isa() const
    { return Tool::isa(); }
    bool isa(std::string const &isa) const override
    { return (Tool::isa(isa) ||
              isa == this->isa() ||
              isa == Tool::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXCompilerSpecificationOpenCL; }
};

} }

#endif  // !__pbxspec_PBX_CompilerSpecificationOpenCL_h
