// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/XC/ToolSpecificationHeadermapGenerator.h>

using pbxspec::XC::ToolSpecificationHeadermapGenerator;

ToolSpecificationHeadermapGenerator::ToolSpecificationHeadermapGenerator(bool isDefault) :
    Tool(isDefault, ISA::XCToolSpecificationHeadermapGenerator)
{
}
