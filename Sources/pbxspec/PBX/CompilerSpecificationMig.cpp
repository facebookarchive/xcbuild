// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/CompilerSpecificationMig.h>

using pbxspec::PBX::CompilerSpecificationMig;

CompilerSpecificationMig::CompilerSpecificationMig(bool isDefault) :
    Compiler(isDefault, ISA::PBXCompilerSpecificationMig)
{
}
