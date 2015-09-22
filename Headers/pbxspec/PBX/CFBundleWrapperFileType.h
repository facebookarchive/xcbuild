// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_CFBundleWrapperFileType_h
#define __pbxspec_PBX_CFBundleWrapperFileType_h

#include <pbxspec/PBX/FileType.h>

namespace pbxspec { namespace PBX {

class CFBundleWrapperFileType : public FileType {
protected:
    friend class FileType;
    CFBundleWrapperFileType();

public:
    inline std::string const &isa() const
    { return FileType::isa(); }
    bool isa(std::string const &isa) const override
    { return (FileType::isa(isa) ||
              isa == this->isa() ||
              isa == FileType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXCFBundleWrapperFileType; }
};

} }

#endif  // !__pbxspec_PBX_CFBundleWrapperFileType_h
