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
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/FileTypeResolver.h>
#include <pbxbuild/HeaderMap.h>
#include <pbxsetting/Type.h>
#include <plist/Array.h>
#include <plist/Dictionary.h>
#include <plist/Integer.h>
#include <plist/String.h>
#include <plist/Format/JSON.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

#include <array>

namespace Tool = pbxbuild::Tool;
using AuxiliaryFile = pbxbuild::Tool::Invocation::AuxiliaryFile;
using pbxbuild::HeaderMap;
using pbxbuild::FileTypeResolver;
using libutil::Filesystem;
using libutil::FSUtil;

Tool::HeadermapResolver::
HeadermapResolver(pbxspec::PBX::Tool::shared_ptr const &tool, pbxspec::PBX::Compiler::shared_ptr const &compiler, pbxspec::Manager::shared_ptr const &specManager) :
    _tool       (tool),
    _compiler   (compiler),
    _specManager(specManager)
{
}

static std::vector<std::string>
HeadermapSearchPaths(pbxspec::Manager::shared_ptr const &specManager, pbxsetting::Environment const &environment, pbxproj::PBX::Target::shared_ptr const &target, Tool::SearchPaths const &searchPaths, std::string const &workingDirectory)
{
    std::unordered_set<std::string> allHeaderSearchPaths;
    std::vector<std::string> orderedHeaderSearchPaths;

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::Type::Sources) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
            if (buildFile->fileRef() == nullptr) {
                continue;
            }

            std::string filePath = environment.expand(buildFile->fileRef()->resolve());
            std::string fullPath = FSUtil::GetDirectoryName(filePath);
            if (allHeaderSearchPaths.insert(fullPath).second) {
                orderedHeaderSearchPaths.push_back(fullPath);
            }
        }
    }

    for (std::string const &path : searchPaths.userHeaderSearchPaths()) {
        std::string fullPath = FSUtil::ResolveRelativePath(path, workingDirectory);
        if (allHeaderSearchPaths.insert(fullPath).second) {
            orderedHeaderSearchPaths.push_back(fullPath);
        }
    }
    for (std::string const &path : searchPaths.headerSearchPaths()) {
        std::string fullPath = FSUtil::ResolveRelativePath(path, workingDirectory);
        if (allHeaderSearchPaths.insert(fullPath).second) {
            orderedHeaderSearchPaths.push_back(fullPath);
        }
    }

    return orderedHeaderSearchPaths;
}

static std::unique_ptr<plist::Dictionary>
VFSFileReference(std::string const &fileName, std::string const &sourcePath)
{
    auto file = plist::Dictionary::New();
    file->set("type", plist::String::New("file"));
    file->set("name", plist::String::New(fileName));
    file->set("external-contents", plist::String::New(sourcePath));
    return file;
}

static std::unique_ptr<plist::Dictionary>
VFSDirectoryReference(std::string const &directoryPath, std::unique_ptr<plist::Array> &&contents)
{
    auto directory = plist::Dictionary::New();
    directory->set("type", plist::String::New("directory"));
    directory->set("name", plist::String::New(directoryPath));
    directory->set("contents", std::move(contents));
    return directory;
}

static std::array<std::unique_ptr<plist::Dictionary>, 2>
VFSHeadersDirectories(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::Target::shared_ptr const &target, std::string const &targetRoot, pbxsetting::Environment const &environment)
{
    std::unique_ptr<plist::Array> publicHeadersContents = plist::Array::New();
    std::unique_ptr<plist::Array> privateHeadersContents = plist::Array::New();

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::Type::Headers) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
            if (buildFile->fileRef() == nullptr || buildFile->fileRef()->type() != pbxproj::PBX::GroupItem::Type::FileReference) {
                continue;
            }

            pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
            std::string filePath = environment.expand(fileReference->resolve());
            pbxspec::PBX::FileType::shared_ptr fileType = FileTypeResolver::Resolve(specManager, { pbxspec::Manager::AnyDomain() }, fileReference, filePath);
            if (fileType == nullptr || (fileType->identifier() != "sourcecode.c.h" && fileType->identifier() != "sourcecode.cpp.h")) {
                continue;
            }

            std::string fileName = FSUtil::GetBaseName(filePath);

            std::vector<std::string> const &attributes = buildFile->attributes();
            bool isPublic  = std::find(attributes.begin(), attributes.end(), "Public") != attributes.end();
            bool isPrivate = std::find(attributes.begin(), attributes.end(), "Private") != attributes.end();

            if (isPublic) {
                std::unique_ptr<plist::Dictionary> file = VFSFileReference(fileName, filePath);
                publicHeadersContents->append(std::move(file));
            } else if (isPrivate) {
                std::unique_ptr<plist::Dictionary> file = VFSFileReference(fileName, filePath);
                privateHeadersContents->append(std::move(file));
            }
        }
    }

    std::string publicHeadersRoot = targetRoot + "/" + environment.resolve("PUBLIC_HEADERS_FOLDER_PATH");
    std::string privateHeadersRoot = targetRoot + "/" + environment.resolve("PRIVATE_HEADERS_FOLDER_PATH");

    std::string swiftObjCInterfaceHeader = environment.resolve("SWIFT_OBJC_INTERFACE_HEADER_NAME");
    if (!swiftObjCInterfaceHeader.empty()) {
        std::string swiftObjCInterfacePath = publicHeadersRoot + "/" + swiftObjCInterfaceHeader;
        std::unique_ptr<plist::Dictionary> swiftObjCInterface = VFSFileReference(swiftObjCInterfaceHeader, swiftObjCInterfacePath);
        publicHeadersContents->append(std::move(swiftObjCInterface));
    }

    std::unique_ptr<plist::Dictionary> publicHeaders = VFSDirectoryReference(publicHeadersRoot, std::move(publicHeadersContents));
    std::unique_ptr<plist::Dictionary> privateHeaders = VFSDirectoryReference(privateHeadersRoot, std::move(privateHeadersContents));
    return {{ std::move(publicHeaders), std::move(privateHeaders) }};
}

static std::unique_ptr<plist::Dictionary>
VFSModulesDirectory(pbxsetting::Environment const &environment, std::string const &targetRoot)
{
    std::string moduleMapSourceRoot = environment.resolve("TARGET_TEMP_DIR");
    std::unique_ptr<plist::Array> modulesContents = plist::Array::New();

    {
        std::string moduleMapName = "module.modulemap";
        std::string moduleMapSource = moduleMapSourceRoot + "/" + moduleMapName;

        std::unique_ptr<plist::Dictionary> moduleMap = VFSFileReference(moduleMapName, moduleMapSource);
        modulesContents->append(std::move(moduleMap));
    }

    std::string privateModuleMapFile = environment.resolve("MODULEMAP_PRIVATE_FILE");
    if (!privateModuleMapFile.empty()) {
        std::string privateModuleMapName = "module.private.modulemap";
        std::string privateModuleMapSource = moduleMapSourceRoot + "/" + privateModuleMapName;

        std::unique_ptr<plist::Dictionary> privateModuleMap = VFSFileReference(privateModuleMapName, privateModuleMapSource);
        modulesContents->append(std::move(privateModuleMap));
    }

    if (!modulesContents->empty()) {
        std::string modulesRoot = targetRoot + "/" + "Modules";
        return VFSDirectoryReference(modulesRoot, std::move(modulesContents));
    } else {
        return nullptr;
    }
}

void Tool::HeadermapResolver::
resolve(
    Tool::Context *toolContext,
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

    bool definesModule = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("DEFINES_MODULE"));
    bool requiresVFS = definesModule || pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_USES_VFS"));

    std::unique_ptr<plist::Dictionary> allProjectHeaders = nullptr;
    if (requiresVFS) {
        // TODO(grp): Create VFS here.
        std::unique_ptr<plist::Array> roots = plist::Array::New();

        // TODO: do this for every target
        std::string targetRoot = compilerEnvironment.resolve("BUILT_PRODUCTS_DIR") + "/" + compilerEnvironment.resolve("WRAPPER_NAME");

        auto headerDirectories = VFSHeadersDirectories(_specManager, target, targetRoot, compilerEnvironment);
        for (auto it = headerDirectories.begin(); it != headerDirectories.end(); ++it) {
            roots->append(std::move(*it));
        }

        if (definesModule) {
            if (std::unique_ptr<plist::Dictionary> modules = VFSModulesDirectory(compilerEnvironment, targetRoot)) {
                roots->append(std::move(modules));
            }
        }

        allProjectHeaders = plist::Dictionary::New();
        allProjectHeaders->set("version", plist::Integer::New(0));
        allProjectHeaders->set("case-sensitive", plist::String::New("false"));
        allProjectHeaders->set("roots", std::move(roots));
    }

    HeaderMap targetName;
    HeaderMap ownTargetHeaders;
    HeaderMap projectHeaders;
    HeaderMap allTargetHeaders;
    HeaderMap allNonFrameworkTargetHeaders;

    bool includeFlatEntriesForTargetBeingBuilt     = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_INCLUDES_FLAT_ENTRIES_FOR_TARGET_BEING_BUILT"));
    bool includeFrameworkEntriesForAllProductTypes = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_INCLUDES_FRAMEWORK_ENTRIES_FOR_ALL_PRODUCT_TYPES"));
    bool includeProjectHeaders                     = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_INCLUDES_PROJECT_HEADERS"));
    // TODO
    // bool useFrameworkPrefixEntries                 = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("HEADERMAP_USES_FRAMEWORK_PREFIX_ENTRIES"));

    // TODO(grp): Populate generated headers.
    HeaderMap generatedFiles;

    pbxproj::PBX::Project::shared_ptr project = target->project();

    std::vector<std::string> headermapSearchPaths = HeadermapSearchPaths(_specManager, compilerEnvironment, target, toolContext->searchPaths(), toolContext->workingDirectory());
    for (std::string const &path : headermapSearchPaths) {
        Filesystem::GetDefaultUNSAFE()->enumerateDirectory(path, [&](std::string const &fileName) -> bool {
            // TODO(grp): Use FileTypeResolver when reliable.
            std::string extension = FSUtil::GetFileExtension(fileName);
            if (extension != "h" && extension != "hpp") {
                return true;
            }

            targetName.add(fileName, path + "/", fileName);
            return true;
        });
    }

    for (pbxproj::PBX::FileReference::shared_ptr const &fileReference : project->fileReferences()) {
        std::string filePath = compilerEnvironment.expand(fileReference->resolve());
        pbxspec::PBX::FileType::shared_ptr fileType = FileTypeResolver::Resolve(Filesystem::GetDefaultUNSAFE(), _specManager, { pbxspec::Manager::AnyDomain() }, fileReference, filePath);
        if (fileType == nullptr || (fileType->identifier() != "sourcecode.c.h" && fileType->identifier() != "sourcecode.cpp.h")) {
            continue;
        }

        std::string fileName = FSUtil::GetBaseName(filePath);
        std::string fileDirectory = FSUtil::GetDirectoryName(filePath) + "/";

        projectHeaders.add(fileName, fileDirectory, fileName);
        if (includeProjectHeaders) {
            targetName.add(fileName, fileDirectory, fileName);
        }
    }

    for (pbxproj::PBX::Target::shared_ptr const &projectTarget : project->targets()) {
       for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : projectTarget->buildPhases()) {
            if (buildPhase->type() != pbxproj::PBX::BuildPhase::Type::Headers) {
                continue;
            }

            for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
                if (buildFile->fileRef() == nullptr || buildFile->fileRef()->type() != pbxproj::PBX::GroupItem::Type::FileReference) {
                    continue;
                }

                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                std::string filePath = compilerEnvironment.expand(fileReference->resolve());
                pbxspec::PBX::FileType::shared_ptr fileType = FileTypeResolver::Resolve(Filesystem::GetDefaultUNSAFE(), _specManager, { pbxspec::Manager::AnyDomain() }, fileReference, filePath);
                if (fileType == nullptr || (fileType->identifier() != "sourcecode.c.h" && fileType->identifier() != "sourcecode.cpp.h")) {
                    continue;
                }

                std::string fileName = FSUtil::GetBaseName(filePath);
                std::string fileDirectory = FSUtil::GetDirectoryName(filePath) + "/";
                std::string frameworkName = projectTarget->productName() + "/" + fileName;

                std::vector<std::string> const &attributes = buildFile->attributes();
                bool isPublic  = std::find(attributes.begin(), attributes.end(), "Public") != attributes.end();
                bool isPrivate = std::find(attributes.begin(), attributes.end(), "Private") != attributes.end();

                if (projectTarget == target) {
                    ownTargetHeaders.add(fileName, fileDirectory, fileName);

                    if (!isPublic && !isPrivate) {
                        ownTargetHeaders.add(frameworkName, fileDirectory, fileName);
                        if (includeFlatEntriesForTargetBeingBuilt) {
                            targetName.add(frameworkName, fileDirectory, fileName);
                        }
                    }
                }

                if (isPublic || isPrivate) {
                    allTargetHeaders.add(frameworkName, fileDirectory, fileName);
                    if (includeFrameworkEntriesForAllProductTypes) {
                        targetName.add(frameworkName, fileDirectory, fileName);
                    }

                    // TODO(grp): This is a little messy. Maybe check the product type specification, or the product reference's file type?
                    if (projectTarget->type() == pbxproj::PBX::Target::Type::Native && std::static_pointer_cast<pbxproj::PBX::NativeTarget>(projectTarget)->productType().find("framework") == std::string::npos) {
                        allNonFrameworkTargetHeaders.add(frameworkName, fileDirectory, fileName);
                        if (!includeFrameworkEntriesForAllProductTypes) {
                            targetName.add(frameworkName, fileDirectory, fileName);
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
        AuxiliaryFile::Data(headermapFile, targetName.write()),
        AuxiliaryFile::Data(headermapFileForOwnTargetHeaders, ownTargetHeaders.write()),
        AuxiliaryFile::Data(headermapFileForAllTargetHeaders, allTargetHeaders.write()),
        AuxiliaryFile::Data(headermapFileForAllNonFrameworkTargetHeaders, allNonFrameworkTargetHeaders.write()),
        AuxiliaryFile::Data(headermapFileForGeneratedFiles, generatedFiles.write()),
        AuxiliaryFile::Data(headermapFileForProjectFiles, projectHeaders.write()),
    };

    std::string vfsAllProductHeaders = compilerEnvironment.resolve("CPP_HEADERMAP_PRODUCT_HEADERS_VFS_FILE");

    if (allProjectHeaders) {
        // TODO(grp): This should be YAML, not JSON, but JSON works for now.
        auto result = plist::Format::JSON::Serialize(allProjectHeaders.get(), plist::Format::JSON::Create());
        if (!result.first) {
            fprintf(stderr, "error: unable to serialize VFS: %s\n", result.second.c_str());
        } else {
            auto auxiliaryFile = AuxiliaryFile::Data(vfsAllProductHeaders, *result.first);
            auxiliaryFiles.push_back(auxiliaryFile);
        }
    }

    Tool::Invocation invocation;
    invocation.auxiliaryFiles().insert(invocation.auxiliaryFiles().end(), auxiliaryFiles.begin(), auxiliaryFiles.end());

    std::vector<std::string> systemHeadermapFiles;
    std::vector<std::string> userHeadermapFiles;

    if (pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("ALWAYS_SEARCH_USER_PATHS")) && !pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("ALWAYS_USE_SEPARATE_HEADERMAPS"))) {
        systemHeadermapFiles.push_back(headermapFile);
    } else {
        if (includeFlatEntriesForTargetBeingBuilt) {
            systemHeadermapFiles.push_back(headermapFileForOwnTargetHeaders);
        }
        if (requiresVFS) {
            systemHeadermapFiles.push_back(headermapFileForAllNonFrameworkTargetHeaders);
        } else {
            systemHeadermapFiles.push_back(headermapFileForAllTargetHeaders);
        }

        userHeadermapFiles.push_back(headermapFileForGeneratedFiles);
        if (includeProjectHeaders) {
            userHeadermapFiles.push_back(headermapFileForProjectFiles);
        }
    }

    toolContext->invocations().push_back(invocation);

    Tool::HeadermapInfo *headermapInfo = &toolContext->headermapInfo();
    headermapInfo->systemHeadermapFiles().insert(headermapInfo->systemHeadermapFiles().end(), systemHeadermapFiles.begin(), systemHeadermapFiles.end());
    headermapInfo->userHeadermapFiles().insert(headermapInfo->userHeadermapFiles().end(), userHeadermapFiles.begin(), userHeadermapFiles.end());
    headermapInfo->overlayVFS() = vfsAllProductHeaders;
}

std::unique_ptr<Tool::HeadermapResolver> Tool::HeadermapResolver::
Create(Phase::Environment const &phaseEnvironment, pbxspec::PBX::Compiler::shared_ptr const &compiler)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr headermapTool = buildEnvironment.specManager()->tool(Tool::HeadermapResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (headermapTool == nullptr) {
        fprintf(stderr, "warning: could not find headermap tool\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::HeadermapResolver>(new Tool::HeadermapResolver(headermapTool, compiler, buildEnvironment.specManager()));
}
