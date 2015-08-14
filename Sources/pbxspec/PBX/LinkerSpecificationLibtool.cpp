// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/LinkerSpecificationLibtool.h>

using pbxspec::PBX::LinkerSpecificationLibtool;

LinkerSpecificationLibtool::LinkerSpecificationLibtool(bool isDefault) :
    Linker(isDefault, ISA::PBXLinkerSpecificationLibtool)
{
}
