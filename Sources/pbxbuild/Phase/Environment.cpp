/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/Environment.h>

namespace Phase = pbxbuild::Phase;

Phase::Environment::
Environment(Build::Environment const &buildEnvironment, Build::Context const &buildContext, pbxproj::PBX::Target::shared_ptr const &target, Target::Environment const &targetEnvironment) :
    _buildEnvironment (buildEnvironment),
    _buildContext     (buildContext),
    _target           (target),
    _targetEnvironment(targetEnvironment)
{
}

Phase::Environment::
~Environment()
{
}

pbxsetting::Level Phase::Environment::
VariantLevel(std::string const &variant)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_VARIANT", variant),
        pbxsetting::Setting::Parse("variant", variant),
        pbxsetting::Setting::Parse("EXECUTABLE_VARIANT_SUFFIX", variant != "normal" ? "_" + variant : ""),
    });
}

pbxsetting::Level Phase::Environment::
ArchitectureLevel(std::string const &arch)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_ARCH", arch),
        pbxsetting::Setting::Parse("arch", arch),
    });
}

