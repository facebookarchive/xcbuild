/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
