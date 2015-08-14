// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/XC/CompilerSpecificationClang.h>

using pbxspec::XC::CompilerSpecificationClang;

CompilerSpecificationClang::CompilerSpecificationClang(bool isDefault) :
    PBX::Compiler(isDefault, ISA::XCCompilerSpecificationClang)
{
}
