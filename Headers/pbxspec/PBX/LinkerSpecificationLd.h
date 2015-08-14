// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_LinkerSpecificationLd_h
#define __pbxspec_PBX_LinkerSpecificationLd_h

#include <pbxspec/PBX/Linker.h>

namespace pbxspec { namespace PBX {

class LinkerSpecificationLd : public Linker {
protected:
    friend class Linker;
    LinkerSpecificationLd(bool isDefault);

public:
    inline std::string const &isa() const
    { return Linker::isa(); }
    bool isa(std::string const &isa) const override
    { return (Linker::isa(isa) ||
              isa == this->isa() ||
              isa == Linker::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXLinkerSpecificationLd; }
};

} }

#endif  // !__pbxspec_PBX_LinkerSpecificationLd_h
