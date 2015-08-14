// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/XC/StaticFrameworkWrapperFileType.h>

using pbxspec::XC::StaticFrameworkWrapperFileType;

StaticFrameworkWrapperFileType::StaticFrameworkWrapperFileType(bool isDefault) :
    PBX::FileType(isDefault, ISA::XCStaticFrameworkWrapperFileType)
{
}
