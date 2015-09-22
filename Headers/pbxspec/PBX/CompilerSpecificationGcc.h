// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_CompilerSpecificationGcc_h
#define __pbxspec_PBX_CompilerSpecificationGcc_h

#include <pbxspec/PBX/Compiler.h>

namespace pbxspec { namespace PBX {

class CompilerSpecificationGcc : public Compiler {
protected:
    friend class Compiler;
    CompilerSpecificationGcc();

public:
    inline std::string const &isa() const
    { return Compiler::isa(); }
    bool isa(std::string const &isa) const override
    { return (Compiler::isa(isa) ||
              isa == this->isa() ||
              isa == Compiler::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXCompilerSpecificationGcc; }
};

} }

#endif  // !__pbxspec_PBX_CompilerSpecificationGcc_h
