// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_CompilerSpecificationClang_h
#define __pbxspec_XC_CompilerSpecificationClang_h

#include <pbxspec/PBX/Compiler.h>

namespace pbxspec { namespace XC {

class CompilerSpecificationClang : public PBX::Compiler {
protected:
    friend class Compiler;
    CompilerSpecificationClang(bool isDefault);

public:
    inline std::string const &isa() const
    { return PBX::Compiler::isa(); }
    bool isa(std::string const &isa) const override
    { return (PBX::Compiler::isa(isa) ||
              isa == this->isa() ||
              isa == PBX::Compiler::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCCompilerSpecificationClang; }
};

} }

#endif  // !__pbxspec_XC_CompilerSpecificationClang_h
