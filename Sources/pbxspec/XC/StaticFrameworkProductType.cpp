// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/XC/StaticFrameworkProductType.h>

using pbxspec::XC::StaticFrameworkProductType;

StaticFrameworkProductType::StaticFrameworkProductType(bool isDefault) :
    ProductType(isDefault, ISA::XCStaticFrameworkProductType)
{
}
