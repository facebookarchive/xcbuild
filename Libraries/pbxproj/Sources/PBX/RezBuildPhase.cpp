/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <pbxproj/PBX/RezBuildPhase.h>

using pbxproj::PBX::RezBuildPhase;

RezBuildPhase::
RezBuildPhase() :
    BuildPhase(Isa(), Type::Rez)
{
}
