// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_StaticFrameworkWrapperFileType_h
#define __pbxspec_XC_StaticFrameworkWrapperFileType_h

#include <pbxspec/PBX/FileType.h>

namespace pbxspec { namespace XC {

class StaticFrameworkWrapperFileType : public PBX::FileType {
protected:
    friend class PBX::FileType;
    StaticFrameworkWrapperFileType();

public:
    inline std::string const &isa() const
    { return PBX::FileType::isa(); }
    bool isa(std::string const &isa) const override
    { return (PBX::FileType::isa(isa) ||
              isa == this->isa() ||
              isa == PBX::FileType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCStaticFrameworkWrapperFileType; }
};

} }

#endif  // !__pbxspec_XC_StaticFrameworkWrapperFileType_h
