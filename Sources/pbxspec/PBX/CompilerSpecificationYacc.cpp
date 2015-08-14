// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/CompilerSpecificationYacc.h>

using pbxspec::PBX::CompilerSpecificationYacc;

CompilerSpecificationYacc::CompilerSpecificationYacc(bool isDefault) :
    Tool(isDefault, ISA::PBXCompilerSpecificationYacc)
{
}
