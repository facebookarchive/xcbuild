// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/CFBundleWrapperFileType.h>

using pbxspec::PBX::CFBundleWrapperFileType;

CFBundleWrapperFileType::CFBundleWrapperFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXCFBundleWrapperFileType)
{
}
