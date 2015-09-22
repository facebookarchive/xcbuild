// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_LinkerSpecificationResMerger_h
#define __pbxspec_PBX_LinkerSpecificationResMerger_h

#include <pbxspec/PBX/Linker.h>

namespace pbxspec { namespace PBX {

class LinkerSpecificationResMerger : public Linker {
protected:
    friend class Linker;
    LinkerSpecificationResMerger();

public:
    inline std::string const &isa() const
    { return Linker::isa(); }
    bool isa(std::string const &isa) const override
    { return (Linker::isa(isa) ||
              isa == this->isa() ||
              isa == Linker::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXLinkerSpecificationResMerger; }
};

} }

#endif  // !__pbxspec_PBX_LinkerSpecificationResMerger_h
