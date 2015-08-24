// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_TargetEnvironment_h
#define __pbxbuild_TargetEnvironment_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>

namespace pbxbuild {

class TargetEnvironment {
private:
    BuildEnvironment _buildEnvironment;

public:
    TargetEnvironment(BuildEnvironment const &buildEnvironment);
    ~TargetEnvironment();

public:
    std::unique_ptr<pbxsetting::Environment>
    targetEnvironment(pbxproj::PBX::Target::shared_ptr const &target, BuildContext const &context) const;
};

}

#endif // !__pbxbuild_DependencyResolver_h
