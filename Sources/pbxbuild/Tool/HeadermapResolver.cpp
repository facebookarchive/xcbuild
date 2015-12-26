/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/HeadermapResolver.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/HeaderMap.h>

using pbxbuild::Tool::HeadermapResolver;
using pbxbuild::Tool::HeadermapInfo;
using pbxbuild::Tool::SearchPaths;
using pbxbuild::Tool::ToolContext;
using pbxbuild::ToolInvocation;
using AuxiliaryFile = pbxbuild::ToolInvocation::AuxiliaryFile;
using pbxbuild::HeaderMap;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

HeadermapResolver::
HeadermapResolver(pbxspec::PBX::Tool::shared_ptr const &tool, pbxspec::PBX::Compiler::shared_ptr const &compiler, pbxspec::Manager::shared_ptr const &specManager) :
    _tool       (tool),
    _compiler   (compiler),
    _specManager(specManager)
{
}

static std::vector<std::string>
HeadermapSearchPaths(pbxspec::Manager::shared_ptr const &specManager, pbxsetting::Environment const &environment, pbxproj::PBX::Target::shared_ptr const &target, SearchPaths const &searchPaths, std::string const &workingDirectory)
{
    std::unordered_set<std::string> allHeaderSearchPaths;
    std::vector<std::string> orderedHeaderSearchPaths;

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeSources) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
            std::string filePath = environment.expand(buildFile->fileRef()->resolve());
            std::string fullPath = FSUtil::GetDirectoryName(filePath);
            if (allHeaderSearchPaths.insert(fullPath).second) {
                orderedHeaderSearchPaths.push_back(fullPath);
            }
        }
    }

    for (std::string const &path : searchPaths.userHeaderSearchPaths()) {
        std::string fullPath = workingDirectory + "/" + path;
        if (allHeaderSearchPaths.insert(fullPath).second) {
            orderedHeaderSearchPaths.push_back(fullPath);
        }
    }
    for (std::string const &path : searchPaths.headerSearchPaths()) {
        std::string fullPath = workingDirectory + "/" + path;
        if (allHeaderSearchPaths.insert(fullPath).second) {
            orderedHeaderSearchPaths.push_back(fullPath);
        }
    }

    return orderedHeaderSearchPaths;
}

void HeadermapResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::Target::shared_ptr const &target
) const
{
    /* Add the compiler default environment, which contains the headermap setting defaults. */
    pbxsetting::Environment compilerEnvironment = environment;
    compilerEnvironment.insertFront(_compiler->defaultSettings(), true);

    if (!pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("USE_HEADERMAP"))) {
        return;
    }

    if (pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_USES_VFS"))) {
        // TODO(grp): Support VFS-based header maps.
    }

    HeaderMap targetName;
    HeaderMap ownTargetHeaders;
    HeaderMap projectHeaders;
    HeaderMap allTargetHeaders;
    HeaderMap allNonFrameworkTargetHeaders;

    bool includeFlatEntriesForTargetBeingBuilt     = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_INCLUDES_FLAT_ENTRIES_FOR_TARGET_BEING_BUILT"));
    bool includeFrameworkEntriesForAllProductTypes = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_INCLUDES_FRAMEWORK_ENTRIES_FOR_ALL_PRODUCT_TYPES"));
    bool includeProjectHeaders                     = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_INCLUDES_PROJECT_HEADERS"));

    // TODO(grp): Populate generated headers.
    HeaderMap generatedFiles;

    pbxproj::PBX::Project::shared_ptr project = target->project();

    std::vector<std::string> headermapSearchPaths = HeadermapSearchPaths(_specManager, compilerEnvironment, target, toolContext->searchPaths(), toolContext->workingDirectory());
    for (std::string const &path : headermapSearchPaths) {
        FSUtil::EnumerateDirectory(path, [&](std::string const &fileName) -> bool {
            // TODO(grp): Use TypeResolvedFile when reliable.
            std::string extension = FSUtil::GetFileExtension(fileName);
            if (extension != "h" && extension != "hpp") {
                return true;
            }

            targetName.add(fileName, path + "/", fileName);
            return true;
        });
    }

    for (pbxproj::PBX::FileReference::shared_ptr const &fileReference : project->fileReferences()) {
        auto file = pbxbuild::TypeResolvedFile::Resolve(_specManager, { pbxspec::Manager::AnyDomain() }, fileReference, compilerEnvironment);
        if (file == nullptr || (file->fileType()->identifier() != "sourcecode.c.h" && file->fileType()->identifier() != "sourcecode.cpp.h")) {
            continue;
        }

        std::string fileName = FSUtil::GetBaseName(file->filePath());
        std::string filePath = FSUtil::GetDirectoryName(file->filePath()) + "/";

        projectHeaders.add(fileName, filePath, fileName);
        if (includeProjectHeaders) {
            targetName.add(fileName, filePath, fileName);
        }
    }

    for (pbxproj::PBX::Target::shared_ptr const &projectTarget : project->targets()) {
       for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : projectTarget->buildPhases()) {
            if (buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeHeaders) {
                continue;
            }

            for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
                if (buildFile->fileRef() == nullptr || buildFile->fileRef()->type() != pbxproj::PBX::GroupItem::kTypeFileReference) {
                    continue;
                }

                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                std::unique_ptr<TypeResolvedFile> file = pbxbuild::TypeResolvedFile::Resolve(_specManager, { pbxspec::Manager::AnyDomain() }, fileReference, compilerEnvironment);
                if (file == nullptr || (file->fileType()->identifier() != "sourcecode.c.h" && file->fileType()->identifier() != "sourcecode.cpp.h")) {
                    continue;
                }

                std::string fileName = FSUtil::GetBaseName(file->filePath());
                std::string filePath = FSUtil::GetDirectoryName(file->filePath()) + "/";
                std::string frameworkName = projectTarget->productName() + "/" + fileName;

                std::vector<std::string> const &attributes = buildFile->attributes();
                bool isPublic  = std::find(attributes.begin(), attributes.end(), "Public") != attributes.end();
                bool isPrivate = std::find(attributes.begin(), attributes.end(), "Private") != attributes.end();

                if (projectTarget == target) {
                    ownTargetHeaders.add(fileName, filePath, fileName);

                    if (!isPublic && !isPrivate) {
                        ownTargetHeaders.add(frameworkName, filePath, fileName);
                        if (includeFlatEntriesForTargetBeingBuilt) {
                            targetName.add(frameworkName, filePath, fileName);
                        }
                    }
                }

                if (isPublic || isPrivate) {
                    allTargetHeaders.add(frameworkName, filePath, fileName);
                    if (includeFrameworkEntriesForAllProductTypes) {
                        targetName.add(frameworkName, filePath, fileName);
                    }

                    // TODO(grp): This is a little messy. Maybe check the product type specification, or the product reference's file type?
                    if (projectTarget->type() == pbxproj::PBX::Target::kTypeNative && std::static_pointer_cast<pbxproj::PBX::NativeTarget>(projectTarget)->productType().find("framework") == std::string::npos) {
                        allNonFrameworkTargetHeaders.add(frameworkName, filePath, fileName);
                        if (!includeFrameworkEntriesForAllProductTypes) {
                            targetName.add(frameworkName, filePath, fileName);
                        }
                    }
                }
            }
        }
    }

    std::string headermapFile                                = compilerEnvironment.resolve("CPP_HEADERMAP_FILE");
    std::string headermapFileForOwnTargetHeaders             = compilerEnvironment.resolve("CPP_HEADERMAP_FILE_FOR_OWN_TARGET_HEADERS");
    std::string headermapFileForAllTargetHeaders             = compilerEnvironment.resolve("CPP_HEADERMAP_FILE_FOR_ALL_TARGET_HEADERS");
    std::string headermapFileForAllNonFrameworkTargetHeaders = compilerEnvironment.resolve("CPP_HEADERMAP_FILE_FOR_ALL_NON_FRAMEWORK_TARGET_HEADERS");
    std::string headermapFileForGeneratedFiles               = compilerEnvironment.resolve("CPP_HEADERMAP_FILE_FOR_GENERATED_FILES");
    std::string headermapFileForProjectFiles                 = compilerEnvironment.resolve("CPP_HEADERMAP_FILE_FOR_PROJECT_FILES");

    std::vector<AuxiliaryFile> auxiliaryFiles = {
        AuxiliaryFile(headermapFile, targetName.write(), false),
        AuxiliaryFile(headermapFileForOwnTargetHeaders, ownTargetHeaders.write(), false),
        AuxiliaryFile(headermapFileForAllTargetHeaders, allTargetHeaders.write(), false),
        AuxiliaryFile(headermapFileForAllNonFrameworkTargetHeaders, allNonFrameworkTargetHeaders.write(), false),
        AuxiliaryFile(headermapFileForGeneratedFiles, generatedFiles.write(), false),
        AuxiliaryFile(headermapFileForProjectFiles, projectHeaders.write(), false),
    };

    ToolInvocation invocation;
    invocation.auxiliaryFiles().insert(invocation.auxiliaryFiles().end(), auxiliaryFiles.begin(), auxiliaryFiles.end());

    std::vector<std::string> systemHeadermapFiles;
    std::vector<std::string> userHeadermapFiles;

    if (!pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("ALWAYS_USE_SEPARATE_HEADERMAPS")) || pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("ALWAYS_SEARCH_USER_PATHS"))) {
        systemHeadermapFiles.push_back(headermapFile);
    } else {
        if (includeFlatEntriesForTargetBeingBuilt) {
            systemHeadermapFiles.push_back(headermapFileForOwnTargetHeaders);
        }
        if (includeFrameworkEntriesForAllProductTypes) {
            systemHeadermapFiles.push_back(headermapFileForAllTargetHeaders);
        } else {
            systemHeadermapFiles.push_back(headermapFileForAllNonFrameworkTargetHeaders);
        }

        userHeadermapFiles.push_back(headermapFileForGeneratedFiles);
        if (includeProjectHeaders) {
            userHeadermapFiles.push_back(headermapFileForProjectFiles);
        }
    }

    toolContext->invocations().push_back(invocation);

    HeadermapInfo *headermapInfo = &toolContext->headermapInfo();
    headermapInfo->systemHeadermapFiles().insert(headermapInfo->systemHeadermapFiles().end(), systemHeadermapFiles.begin(), systemHeadermapFiles.end());
    headermapInfo->userHeadermapFiles().insert(headermapInfo->userHeadermapFiles().end(), userHeadermapFiles.begin(), userHeadermapFiles.end());
}

std::unique_ptr<HeadermapResolver> HeadermapResolver::
Create(Phase::Environment const &phaseEnvironment, pbxspec::PBX::Compiler::shared_ptr const &compiler)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr headermapTool = buildEnvironment.specManager()->tool(HeadermapResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (headermapTool == nullptr) {
        fprintf(stderr, "warning: could not find headermap tool\n");
        return nullptr;
    }

    return std::unique_ptr<HeadermapResolver>(new HeadermapResolver(headermapTool, compiler, buildEnvironment.specManager()));
}
