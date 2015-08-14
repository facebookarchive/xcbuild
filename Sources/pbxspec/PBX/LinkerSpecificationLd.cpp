// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/LinkerSpecificationLd.h>

using pbxspec::PBX::LinkerSpecificationLd;

LinkerSpecificationLd::LinkerSpecificationLd(bool isDefault) :
    Linker(isDefault, ISA::PBXLinkerSpecificationLd)
{
}
