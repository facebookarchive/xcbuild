// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/DynamicLibraryProductType.h>

using pbxspec::PBX::DynamicLibraryProductType;

DynamicLibraryProductType::DynamicLibraryProductType(bool isDefault) :
    ProductType(isDefault, ISA::PBXDynamicLibraryProductType)
{
}
