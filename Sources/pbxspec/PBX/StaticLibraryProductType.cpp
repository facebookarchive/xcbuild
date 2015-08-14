// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/StaticLibraryProductType.h>

using pbxspec::PBX::StaticLibraryProductType;

StaticLibraryProductType::StaticLibraryProductType(bool isDefault) :
    ProductType(isDefault, ISA::PBXStaticLibraryProductType)
{
}
