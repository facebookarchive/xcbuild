// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_FrameworkWrapperFileType_h
#define __pbxspec_PBX_FrameworkWrapperFileType_h

#include <pbxspec/PBX/FileType.h>

namespace pbxspec { namespace PBX {

class FrameworkWrapperFileType : public FileType {
protected:
    friend class FileType;
    FrameworkWrapperFileType(bool isDefault);

public:
    inline std::string const &isa() const
    { return FileType::isa(); }
    bool isa(std::string const &isa) const override
    { return (FileType::isa(isa) ||
              isa == this->isa() ||
              isa == FileType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXFrameworkWrapperFileType; }
};

} }

#endif  // !__pbxspec_PBX_FrameworkWrapperFileType_h
