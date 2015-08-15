// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/SpotlightImporternWrapperFileType.h>

using pbxspec::PBX::SpotlightImporternWrapperFileType;

SpotlightImporternWrapperFileType::SpotlightImporternWrapperFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXSpotlightImporternWrapperFileType)
{
}
