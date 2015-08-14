// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/SourcesBuildPhase.h>

using pbxproj::PBX::SourcesBuildPhase;

SourcesBuildPhase::SourcesBuildPhase() :
    BuildPhase(Isa(), kTypeSources)
{
}
