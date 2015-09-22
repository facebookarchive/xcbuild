// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_PlugInKitPluginWrapperFileType_h
#define __pbxspec_PBX_PlugInKitPluginWrapperFileType_h

#include <pbxspec/PBX/FileType.h>

namespace pbxspec { namespace PBX {

class PlugInKitPluginWrapperFileType : public FileType {
protected:
    friend class FileType;
    PlugInKitPluginWrapperFileType();

public:
    inline std::string const &isa() const
    { return FileType::isa(); }
    bool isa(std::string const &isa) const override
    { return (FileType::isa(isa) ||
              isa == this->isa() ||
              isa == FileType::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::PBXPlugInKitPluginWrapperFileType; }
};

} }

#endif  // !__pbxspec_PBX_PlugInKitPluginWrapperFileType_h
