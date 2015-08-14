// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_CompilerSpecificationLex_h
#define __pbxspec_PBX_CompilerSpecificationLex_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace PBX {

class CompilerSpecificationLex : public Tool {
protected:
    friend class Tool;
    CompilerSpecificationLex(bool isDefault);

public:
    inline std::string const &isa() const
    { return Tool::isa(); }
    bool isa(std::string const &isa) const override
    { return (Tool::isa(isa) ||
              isa == this->isa() ||
              isa == Tool::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXCompilerSpecificationLex; }
};

} }

#endif  // !__pbxspec_PBX_CompilerSpecificationLex_h
