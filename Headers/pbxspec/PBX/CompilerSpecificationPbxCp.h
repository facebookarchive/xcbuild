// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_CompilerSpecificationPbxCp_h
#define __pbxspec_PBX_CompilerSpecificationPbxCp_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace PBX {

class CompilerSpecificationPbxCp : public Tool {
protected:
    friend class Tool;
    CompilerSpecificationPbxCp(bool isDefault);

public:
    inline std::string const &isa() const
    { return Tool::isa(); }
    bool isa(std::string const &isa) const override
    { return (Tool::isa(isa) ||
              isa == this->isa() ||
              isa == Tool::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXCompilerSpecificationPbxCp; }
};

} }

#endif  // !__pbxspec_PBX_CompilerSpecificationPbxCp_h
