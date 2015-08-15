// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/PlugInKitPluginWrapperFileType.h>

using pbxspec::PBX::PlugInKitPluginWrapperFileType;

PlugInKitPluginWrapperFileType::PlugInKitPluginWrapperFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXPlugInKitPluginWrapperFileType)
{
}
