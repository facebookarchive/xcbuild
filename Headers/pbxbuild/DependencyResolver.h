// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_DependencyResolver_h
#define __pbxbuild_DependencyResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildGraph.h>
#include <pbxbuild/BuildContext.h>

namespace pbxbuild {

class DependencyResolver {
private:
    BuildEnvironment _buildEnvironment;

public:
    DependencyResolver(BuildEnvironment const &buildEnviroment);
    ~DependencyResolver();

public:
    BuildGraph<pbxproj::PBX::Target::shared_ptr>
    resolveSchemeDependencies(BuildContext const &context) const;

public:
    BuildGraph<pbxproj::PBX::Target::shared_ptr>
    resolveLegacyDependencies(BuildContext const &context, bool allTargets, std::string const &target) const;
};

}

#endif // !__pbxbuild_DependencyResolver_h
