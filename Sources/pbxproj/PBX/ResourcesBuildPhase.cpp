// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/ResourcesBuildPhase.h>

using pbxproj::PBX::ResourcesBuildPhase;

ResourcesBuildPhase::ResourcesBuildPhase() :
    BuildPhase(Isa(), kTypeResources)
{
}
