/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ModuleMapResolver.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxsetting/Environment.h>
#include <pbxsetting/Type.h>
#include <libutil/FSUtil.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Phase::ModuleMapResolver::
ModuleMapResolver()
{
}

Phase::ModuleMapResolver::
~ModuleMapResolver()
{
}

static ext::optional<std::string>
FindUmbrellaHeaderName(pbxsetting::Environment const &environment, std::string const &moduleName, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::string lowerModuleName;
    std::transform(moduleName.begin(), moduleName.end(), std::back_inserter(lowerModuleName), ::tolower);

    ext::optional<std::string> umbrellaHeader;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::Type::Headers) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
            if (buildFile->fileRef() == nullptr) {
                continue;
            }

            std::string filePath = environment.expand(buildFile->fileRef()->resolve());
            if (FSUtil::GetFileExtension(filePath) == "h") {
                std::string baseName = FSUtil::GetBaseNameWithoutExtension(filePath);

                std::string lowerBaseName;
                std::transform(baseName.begin(), baseName.end(), std::back_inserter(lowerBaseName), ::tolower);

                if (lowerBaseName == lowerModuleName) {
                    return baseName;
                }
            }
        }
    }

    return ext::nullopt;
}

ext::optional<Tool::Invocation::AuxiliaryFile::Chunk> Phase::ModuleMapResolver::
Contents(pbxsetting::Environment const &environment, pbxproj::PBX::Target::shared_ptr const &target, std::string const &workingDirectory)
{
    std::string inputModuleMapPath = environment.resolve("MODULEMAP_FILE");

    if (!inputModuleMapPath.empty()) {
        /*
         * Copy in the input module map as an auxiliary file.
         */
        std::string path = FSUtil::ResolveRelativePath(inputModuleMapPath, workingDirectory);
        return Tool::Invocation::AuxiliaryFile::Chunk::File(path);
    } else {
        /*
         * Find umbrella header: a header with the same name as the target's module.
         */
        std::string moduleName = environment.resolve("PRODUCT_MODULE_NAME");
        ext::optional<std::string> umbrellaHeaderName = FindUmbrellaHeaderName(environment, moduleName, target);

        if (umbrellaHeaderName) {
            /*
             * Generate a module map as an auxiliary file.
             */
            auto contents = std::string();
            contents += "framework module " + moduleName + " {\n";
            contents += "  umbrella header \"" + *umbrellaHeaderName + "\"\n";
            contents += "  \n";
            contents += "  export *\n";
            contents += "  module * { export * }\n";
            contents += "}\n";

            auto data = std::vector<uint8_t>(contents.begin(), contents.end());
            return Tool::Invocation::AuxiliaryFile::Chunk::Data(data);
        } else {
            return ext::nullopt;
        }
    }
}

bool Phase::ModuleMapResolver::
resolve(Phase::Environment const &phaseEnvironment, Phase::Context *phaseContext) const
{
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();
    std::string const &workingDirectory = phaseContext->toolContext().workingDirectory();

    /*
     * Check if creating module maps is even requested.
     */
    if (!pbxsetting::Type::ParseBoolean(environment.resolve("DEFINES_MODULE"))) {
        return true;
    }

    std::string intermediateModuleMapRoot = environment.resolve("TARGET_TEMP_DIR");
    // TODO: what about non-framework product types?
    std::string finalModuleMapRoot = environment.resolve("TARGET_BUILD_DIR") + "/" + environment.resolve("CONTENTS_FOLDER_PATH") + "/" + "Modules";

    /*
     * Handle the standard module map, either copied or generated.
     */
    ext::optional<Tool::Invocation::AuxiliaryFile::Chunk> contents = Contents(environment, phaseEnvironment.target(), workingDirectory);
    if (contents) {
        std::string intermediateModuleMapPath = intermediateModuleMapRoot + "/" + "module.modulemap";
        auto moduleMapAuxiliaryFile = Tool::Invocation::AuxiliaryFile(intermediateModuleMapPath, { *contents });

        /* Define source module map. */
        // TODO: it would be nicer to attach this to the copy invocation created below
        Tool::Invocation invocation;
        invocation.auxiliaryFiles().push_back(moduleMapAuxiliaryFile);
        phaseContext->toolContext().invocations().push_back(invocation);

        /* Copy in the module map as part of the build. */
        if (Tool::CopyResolver const *copyResolver = phaseContext->copyResolver(phaseEnvironment)) {
            copyResolver->resolve(&phaseContext->toolContext(), environment, { intermediateModuleMapPath }, finalModuleMapRoot, "Ditto");
        } else {
            fprintf(stderr, "warning: failed to get copy tool for module map\n");
        }
    } else {
        fprintf(stderr, "warning: target defines module, but has no umbrella header\n");
    }

    /*
     * Handle the private module map. This module map can only be specified; it is never auto-generated.
     */
    std::string inputPrivateModuleMapPath = environment.resolve("MODULEMAP_PRIVATE_FILE");
    if (!inputPrivateModuleMapPath.empty()) {
        /*
         * Copy in the private module map.
         */
        std::string intermediatePrivateModuleMapPath = intermediateModuleMapRoot + "/" + "module.private.modulemap";
        auto privateModuleMapAuxiliaryFile = Tool::Invocation::AuxiliaryFile::File(
            intermediatePrivateModuleMapPath,
            FSUtil::ResolveRelativePath(inputPrivateModuleMapPath, workingDirectory));

        /* Define source module map. */
        // TODO: it would be nicer to attach this to the copy invocation created below
        Tool::Invocation invocation;
        invocation.auxiliaryFiles().push_back(privateModuleMapAuxiliaryFile);
        phaseContext->toolContext().invocations().push_back(invocation);

        /* Copy in the module map as part of the build. */
        if (Tool::CopyResolver const *copyResolver = phaseContext->copyResolver(phaseEnvironment)) {
            copyResolver->resolve(&phaseContext->toolContext(), environment, { intermediatePrivateModuleMapPath }, finalModuleMapRoot, "Ditto");
        } else {
            fprintf(stderr, "warning: failed to get copy tool for module map\n");
        }
    }

    return true;
}

