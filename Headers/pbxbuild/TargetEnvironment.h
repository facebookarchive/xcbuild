// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_TargetEnvironment_h
#define __pbxbuild_TargetEnvironment_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildContext.h>

namespace pbxbuild {

class TargetEnvironment {
private:
    BuildContext::shared_ptr _buildContext;

public:
    TargetEnvironment(BuildContext::shared_ptr const &context);
    ~TargetEnvironment();

public:
    std::unique_ptr<pbxsetting::Environment>
    targetEnvironment(pbxproj::PBX::Target::shared_ptr const &target, std::string const &configuration) const;
};

}

#endif // !__pbxbuild_DependencyResolver_h
