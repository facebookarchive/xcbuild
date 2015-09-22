// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_LinkerSpecificationLibtool_h
#define __pbxspec_PBX_LinkerSpecificationLibtool_h

#include <pbxspec/PBX/Linker.h>

namespace pbxspec { namespace PBX {

class LinkerSpecificationLibtool : public Linker {
protected:
    friend class Linker;
    LinkerSpecificationLibtool();

public:
    inline std::string const &isa() const
    { return Linker::isa(); }
    bool isa(std::string const &isa) const override
    { return (Linker::isa(isa) ||
              isa == this->isa() ||
              isa == Linker::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXLinkerSpecificationLibtool; }
};

} }

#endif  // !__pbxspec_PBX_LinkerSpecificationLibtool_h
