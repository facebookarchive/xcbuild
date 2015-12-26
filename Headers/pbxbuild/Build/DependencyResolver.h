/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Build_DependencyResolver_h
#define __pbxbuild_Build_DependencyResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Build/Environment.h>
#include <pbxbuild/Build/Context.h>
#include <pbxbuild/DirectedGraph.h>

namespace pbxbuild {
namespace Build {

class DependencyResolver {
private:
    Build::Environment _buildEnvironment;

public:
    DependencyResolver(Build::Environment const &buildEnviroment);
    ~DependencyResolver();

public:
    DirectedGraph<pbxproj::PBX::Target::shared_ptr>
    resolveSchemeDependencies(Build::Context const &context) const;

public:
    DirectedGraph<pbxproj::PBX::Target::shared_ptr>
    resolveLegacyDependencies(Build::Context const &context, bool allTargets, std::string const &target) const;
};

}
}

#endif // !__pbxbuild_Build_DependencyResolver_h
