// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/XPCServiceWrapperFileType.h>

using pbxspec::PBX::XPCServiceWrapperFileType;

XPCServiceWrapperFileType::XPCServiceWrapperFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXXPCServiceWrapperFileType)
{
}
