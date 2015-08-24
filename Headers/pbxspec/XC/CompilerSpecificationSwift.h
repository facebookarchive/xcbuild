// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_CompilerSpecificationSwift_h
#define __pbxspec_XC_CompilerSpecificationSwift_h

#include <pbxspec/PBX/Compiler.h>

namespace pbxspec { namespace XC {

class CompilerSpecificationSwift : public PBX::Compiler {
protected:
    friend class Compiler;
    CompilerSpecificationSwift(bool isDefault);

public:
    inline std::string const &isa() const
    { return PBX::Compiler::isa(); }
    bool isa(std::string const &isa) const override
    { return (PBX::Compiler::isa(isa) ||
              isa == this->isa() ||
              isa == PBX::Compiler::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCCompilerSpecificationSwift; }
};

} }

#endif  // !__pbxspec_XC_CompilerSpecificationSwift_h
