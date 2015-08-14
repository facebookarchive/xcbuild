// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_CompilerSpecificationMig_h
#define __pbxspec_PBX_CompilerSpecificationMig_h

#include <pbxspec/PBX/Compiler.h>

namespace pbxspec { namespace PBX {

class CompilerSpecificationMig : public Compiler {
protected:
    friend class Compiler;
    CompilerSpecificationMig(bool isDefault);

public:
    inline std::string const &isa() const
    { return Compiler::isa(); }
    bool isa(std::string const &isa) const override
    { return (Compiler::isa(isa) ||
              isa == this->isa() ||
              isa == Compiler::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXCompilerSpecificationMig; }
};

} }

#endif  // !__pbxspec_PBX_CompilerSpecificationMig_h
