// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/FrameworkWrapperFileType.h>

using pbxspec::PBX::FrameworkWrapperFileType;

FrameworkWrapperFileType::FrameworkWrapperFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXFrameworkWrapperFileType)
{
}
