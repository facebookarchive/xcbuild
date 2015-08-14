// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/ApplicationProductType.h>

using pbxspec::PBX::ApplicationProductType;

ApplicationProductType::ApplicationProductType(bool isDefault) :
    ProductType(isDefault, ISA::PBXApplicationProductType)
{
}
