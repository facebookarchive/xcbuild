// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/MachOFileType.h>

using pbxspec::PBX::MachOFileType;

MachOFileType::MachOFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXMachOFileType)
{
}
