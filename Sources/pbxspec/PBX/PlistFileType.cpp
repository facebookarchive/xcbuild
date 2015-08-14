// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/PlistFileType.h>

using pbxspec::PBX::PlistFileType;

PlistFileType::PlistFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXPlistFileType)
{
}
