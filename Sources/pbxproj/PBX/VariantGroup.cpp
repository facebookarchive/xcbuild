// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/VariantGroup.h>

using pbxproj::PBX::VariantGroup;

VariantGroup::VariantGroup() :
    BaseGroup(Isa(), GroupItem::kTypeVariantGroup)
{
}
