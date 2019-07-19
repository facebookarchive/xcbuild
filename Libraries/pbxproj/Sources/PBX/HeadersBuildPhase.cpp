/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <pbxproj/PBX/HeadersBuildPhase.h>

using pbxproj::PBX::HeadersBuildPhase;

HeadersBuildPhase::
HeadersBuildPhase() :
    BuildPhase(Isa(), Type::Headers)
{
}
