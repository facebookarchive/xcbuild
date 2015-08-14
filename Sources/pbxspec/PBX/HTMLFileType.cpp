// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/HTMLFileType.h>

using pbxspec::PBX::HTMLFileType;

HTMLFileType::HTMLFileType(bool isDefault) :
    FileType(isDefault, ISA::PBXHTMLFileType)
{
}
