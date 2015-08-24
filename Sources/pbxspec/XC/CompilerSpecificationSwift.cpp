// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/XC/CompilerSpecificationSwift.h>

using pbxspec::XC::CompilerSpecificationSwift;

CompilerSpecificationSwift::CompilerSpecificationSwift(bool isDefault) :
    PBX::Compiler(isDefault, ISA::XCCompilerSpecificationSwift)
{
}
