// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/HeaderMap.h>

using pbxbuild::Tool::HeadermapInvocationContext;
using pbxbuild::ToolInvocation;
using AuxiliaryFile = pbxbuild::ToolInvocation::AuxiliaryFile;
using pbxbuild::HeaderMap;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

HeadermapInvocationContext::
HeadermapInvocationContext(ToolInvocation const &invocation, std::vector<std::string> const &systemHeadermapFiles, std::vector<std::string> const &userHeadermapFiles) :
    _invocation          (invocation),
    _systemHeadermapFiles(systemHeadermapFiles),
    _userHeadermapFiles  (userHeadermapFiles)
{
}

HeadermapInvocationContext::
~HeadermapInvocationContext()
{
}

HeadermapInvocationContext HeadermapInvocationContext::
Create(
    pbxspec::PBX::Tool::shared_ptr const &headermapTool,
    pbxspec::PBX::Compiler::shared_ptr const &defaultCompiler,
    pbxspec::Manager::shared_ptr const &specManager,
    pbxproj::PBX::Target::shared_ptr const &target,
    SearchPaths const &searchPaths,
    pbxsetting::Environment const &environment
)
{
    pbxsetting::Environment env = environment;
    env.insertFront(defaultCompiler->defaultSettings(), true);

    if (!pbxsetting::Type::ParseBoolean(env.resolve("USE_HEADERMAP"))) {
        return HeadermapInvocationContext(ToolInvocation({ }, { }, { }), { }, { });
    }

    if (pbxsetting::Type::ParseBoolean(env.resolve("HEADERMAP_USES_VFS"))) {
        // TODO(grp): Support VFS-based header maps.
    }

    HeaderMap targetName;
    HeaderMap ownTargetHeaders;
    HeaderMap projectHeaders;
    HeaderMap allTargetHeaders;
    HeaderMap allNonFrameworkTargetHeaders;

    // TODO(grp): Populate generated headers.
    HeaderMap generatedFiles;

    pbxproj::PBX::Project::shared_ptr project = target->project();

    for (pbxproj::PBX::FileReference::shared_ptr const &fileReference : project->fileReferences()) {
        auto file = pbxbuild::TypeResolvedFile::Resolve(specManager, fileReference, env);
        if (file == nullptr || (file->fileType()->identifier() != "sourcecode.c.h" && file->fileType()->identifier() != "sourcecode.cpp.h")) {
            continue;
        }

        std::string fileName = FSUtil::GetBaseName(file->filePath());
        std::string filePath = FSUtil::GetDirectoryName(file->filePath()) + "/";

        targetName.add(fileName, filePath, fileName);
        projectHeaders.add(fileName, filePath, fileName);
    }

    for (pbxproj::PBX::Target::shared_ptr const &projectTarget : project->targets()) {
       for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : projectTarget->buildPhases()) {
            if (buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeHeaders) {
                continue;
            }

            for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
                auto file = pbxbuild::TypeResolvedFile::Resolve(specManager, buildFile->fileReference(), env);
                if (file == nullptr || (file->fileType()->identifier() != "sourcecode.c.h" && file->fileType()->identifier() != "sourcecode.cpp.h")) {
                    continue;
                }

                std::string fileName = FSUtil::GetBaseName(file->filePath());
                std::string filePath = FSUtil::GetDirectoryName(file->filePath()) + "/";
                std::string frameworkName = projectTarget->name() + "/" + fileName;

                std::vector<std::string> const &attributes = buildFile->attributes();
                bool isPublic  = std::find(attributes.begin(), attributes.end(), "Public") != attributes.end();
                bool isPrivate = std::find(attributes.begin(), attributes.end(), "Private") != attributes.end();

                if (projectTarget == target) {
                    ownTargetHeaders.add(fileName, filePath, fileName);

                    if (!isPublic && !isPrivate) {
                        ownTargetHeaders.add(frameworkName, filePath, fileName);
                        targetName.add(frameworkName, filePath, fileName);
                    }
                }

                if (isPublic || isPrivate) {
                    targetName.add(frameworkName, filePath, fileName);
                    allTargetHeaders.add(frameworkName, filePath, fileName);

                    // TODO(grp): This is a little messy. Maybe check the product type specification, or the product reference's file type?
                    if (projectTarget->type() == pbxproj::PBX::Target::kTypeNative && std::static_pointer_cast<pbxproj::PBX::NativeTarget>(projectTarget)->productType().find("framework") == std::string::npos) {
                        allNonFrameworkTargetHeaders.add(frameworkName, filePath, fileName);
                    }
                }
            }
        }
    }

    ToolInvocation invocation = ToolInvocation({ }, { }, {
        AuxiliaryFile(env.resolve("CPP_HEADERMAP_FILE"), targetName.write(), false),
        AuxiliaryFile(env.resolve("CPP_HEADERMAP_FILE_FOR_OWN_TARGET_HEADERS"), ownTargetHeaders.write(), false),
        AuxiliaryFile(env.resolve("CPP_HEADERMAP_FILE_FOR_ALL_TARGET_HEADERS"), allTargetHeaders.write(), false),
        AuxiliaryFile(env.resolve("CPP_HEADERMAP_FILE_FOR_ALL_NON_FRAMEWORK_TARGET_HEADERS"), allNonFrameworkTargetHeaders.write(), false),
        AuxiliaryFile(env.resolve("CPP_HEADERMAP_FILE_FOR_GENERATED_FILES"), generatedFiles.write(), false),
        AuxiliaryFile(env.resolve("CPP_HEADERMAP_FILE_FOR_PROJECT_FILES"), projectHeaders.write(), false),
    });

    std::vector<std::string> systemHeadermapFiles;
    std::vector<std::string> userHeadermapFiles;

    if (!pbxsetting::Type::ParseBoolean(env.resolve("ALWAYS_USE_SEPARATE_HEADERMAPS"))) {
        systemHeadermapFiles.push_back(env.resolve("CPP_HEADERMAP_FILE"));
    } else {
        if (pbxsetting::Type::ParseBoolean(env.resolve("HEADERMAP_INCLUDES_FLAT_ENTRIES_FOR_TARGET_BEING_BUILT"))) {
            systemHeadermapFiles.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_OWN_TARGET_HEADERS"));
        }
        if (pbxsetting::Type::ParseBoolean(env.resolve("HEADERMAP_INCLUDES_FRAMEWORK_ENTRIES_FOR_ALL_PRODUCT_TYPES"))) {
            systemHeadermapFiles.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_ALL_TARGET_HEADERS"));
        } else {
            systemHeadermapFiles.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_ALL_NON_FRAMEWORK_TARGET_HEADERS"));
        }

        userHeadermapFiles.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_GENERATED_FILES"));
        if (pbxsetting::Type::ParseBoolean(env.resolve("HEADERMAP_INCLUDES_PROJECT_HEADERS"))) {
            userHeadermapFiles.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_PROJECT_FILES"));
        }
    }

    return HeadermapInvocationContext(invocation, systemHeadermapFiles, userHeadermapFiles);
}
