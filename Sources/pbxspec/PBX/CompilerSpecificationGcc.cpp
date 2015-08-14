// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/CompilerSpecificationGcc.h>

using pbxspec::PBX::CompilerSpecificationGcc;

CompilerSpecificationGcc::CompilerSpecificationGcc(bool isDefault) :
    Compiler(isDefault, ISA::PBXCompilerSpecificationGcc)
{
}
