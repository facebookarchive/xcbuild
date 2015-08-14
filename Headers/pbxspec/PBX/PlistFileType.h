// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_PlistFileType_h
#define __pbxspec_PBX_PlistFileType_h

#include <pbxspec/PBX/FileType.h>

namespace pbxspec { namespace PBX {

class PlistFileType : public FileType {
protected:
    friend class FileType;
    PlistFileType(bool isDefault);

public:
    inline std::string const &isa() const
    { return FileType::isa(); }
    bool isa(std::string const &isa) const override
    { return (FileType::isa(isa) ||
              isa == this->isa() ||
              isa == FileType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXPlistFileType; }
};

} }

#endif  // !__pbxspec_PBX_PlistFileType_h
