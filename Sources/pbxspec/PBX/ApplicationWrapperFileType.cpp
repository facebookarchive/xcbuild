// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/ApplicationWrapperFileType.h>

using pbxspec::PBX::ApplicationWrapperFileType;

ApplicationWrapperFileType::ApplicationWrapperFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXApplicationWrapperFileType)
{
}
