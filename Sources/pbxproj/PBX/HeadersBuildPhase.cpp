// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/HeadersBuildPhase.h>

using pbxproj::PBX::HeadersBuildPhase;

HeadersBuildPhase::HeadersBuildPhase() :
    BuildPhase(Isa(), kTypeHeaders)
{
}
