/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_AssetCatalogResolver_h
#define __pbxbuild_Tool_AssetCatalogResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/File.h>

namespace pbxbuild {
namespace Tool {

class Context;

class AssetCatalogResolver {
private:
    pbxspec::PBX::Compiler::shared_ptr _tool;

private:
    explicit AssetCatalogResolver(pbxspec::PBX::Compiler::shared_ptr const &tool);

public:
    void resolve(
        Tool::Context *toolContext,
        pbxsetting::Environment const &environment,
        std::vector<Tool::Input> const &input) const;

public:
    static std::string ToolIdentifier()
    { return "com.apple.compilers.assetcatalog"; }

public:
    static std::unique_ptr<AssetCatalogResolver>
    Create(Phase::Environment const &phaseEnvironment);
};

}
}

#endif // !__pbxbuild_Tool_AssetCatalogResolver_h
