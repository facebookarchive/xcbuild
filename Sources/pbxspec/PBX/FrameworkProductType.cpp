// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/FrameworkProductType.h>

using pbxspec::PBX::FrameworkProductType;

FrameworkProductType::FrameworkProductType(bool isDefault) :
    ProductType(isDefault, ISA::PBXFrameworkProductType)
{
}
