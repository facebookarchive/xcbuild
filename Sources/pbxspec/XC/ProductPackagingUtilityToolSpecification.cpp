// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/XC/ProductPackagingUtilityToolSpecification.h>

using pbxspec::XC::ProductPackagingUtilityToolSpecification;

ProductPackagingUtilityToolSpecification::ProductPackagingUtilityToolSpecification(bool isDefault) :
    Tool(isDefault, ISA::XCProductPackagingUtilityToolSpecification)
{
}
