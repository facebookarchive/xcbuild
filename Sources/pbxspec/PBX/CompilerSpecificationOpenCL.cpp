// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/CompilerSpecificationOpenCL.h>

using pbxspec::PBX::CompilerSpecificationOpenCL;

CompilerSpecificationOpenCL::CompilerSpecificationOpenCL(bool isDefault) :
    Tool(isDefault, ISA::PBXCompilerSpecificationOpenCL)
{
}
