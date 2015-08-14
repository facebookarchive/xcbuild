// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/CompilerSpecificationLex.h>

using pbxspec::PBX::CompilerSpecificationLex;

CompilerSpecificationLex::CompilerSpecificationLex(bool isDefault) :
    Tool(isDefault, ISA::PBXCompilerSpecificationLex)
{
}
