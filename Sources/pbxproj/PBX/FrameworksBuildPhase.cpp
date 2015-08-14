// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/FrameworksBuildPhase.h>

using pbxproj::PBX::FrameworksBuildPhase;

FrameworksBuildPhase::FrameworksBuildPhase() :
    BuildPhase(Isa(), kTypeFrameworks)
{
}
