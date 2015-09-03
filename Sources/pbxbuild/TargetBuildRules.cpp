// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/TargetBuildRules.h>

using pbxbuild::TargetBuildRules;
using libutil::FSUtil;
using libutil::Wildcard;

TargetBuildRules::BuildRule::
BuildRule(std::string const &filePatterns, pbxspec::PBX::FileType::vector const &fileTypes, pbxspec::PBX::Tool::shared_ptr const &tool, std::string const &script, std::vector<pbxsetting::Value> const &outputFiles) :
    _filePatterns(filePatterns),
    _fileTypes   (fileTypes),
    _tool        (tool),
    _script      (script),
    _outputFiles (outputFiles)
{
}

TargetBuildRules::
TargetBuildRules(TargetBuildRules::BuildRule::vector const &buildRules) :
    _buildRules(buildRules)
{
}

TargetBuildRules::BuildRule::shared_ptr TargetBuildRules::
resolve(pbxbuild::FileTypeResolver const &file) const
{
    for (BuildRule::shared_ptr const &buildRule : _buildRules) {
        if (!buildRule->filePatterns().empty()) {
            if (Wildcard::Match(buildRule->filePatterns(), FSUtil::GetBaseName(file.filePath()))) {
                return buildRule;
            }
        } else {
            pbxspec::PBX::FileType::vector fileTypes = buildRule->fileTypes();
            for (pbxspec::PBX::FileType::shared_ptr fileType = file.fileType(); fileType != nullptr; fileType = fileType->base()) {
                if (std::find(fileTypes.begin(), fileTypes.end(), fileType) != fileTypes.end()) {
                    return buildRule;
                }
            }
        }
    }

    return nullptr;
}

static TargetBuildRules::BuildRule::shared_ptr
ProjectBuildRule(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::BuildRule::shared_ptr const &projBuildRule)
{
    pbxspec::PBX::Tool::shared_ptr tool = nullptr;
    std::string TS = projBuildRule->compilerSpec();
    if (TS != "com.apple.compilers.proxy.script") {
        tool = specManager->tool(TS) ?: std::static_pointer_cast<pbxspec::PBX::Tool>(specManager->compiler(TS)) ?: std::static_pointer_cast<pbxspec::PBX::Tool>(specManager->linker(TS));
        if (tool == nullptr) {
            fprintf(stderr, "warning: couldn't find tool %s specified in build rule\n", TS.c_str());
            return nullptr;
        }
    }

    pbxspec::PBX::FileType::vector fileTypes;
    std::string FT = projBuildRule->fileType();
    if (FT != "pattern.proxy") {
        pbxspec::PBX::FileType::shared_ptr fileType = specManager->fileType(FT);
        if (fileType == nullptr) {
            fprintf(stderr, "warning: couldn't find input file type %s specified in build rule\n", FT.c_str());
            return nullptr;
        }
        fileTypes.push_back(fileType);
    }

    std::vector<pbxsetting::Value> outputFiles;
    for (std::string const &outputFile : projBuildRule->outputFiles()) {
        outputFiles.push_back(pbxsetting::Value::Parse(outputFile));
    }

    return std::make_shared <TargetBuildRules::BuildRule> (TargetBuildRules::BuildRule(
        projBuildRule->filePatterns(),
        fileTypes,
        tool,
        projBuildRule->script(),
        outputFiles
    ));
}

static TargetBuildRules::BuildRule::shared_ptr
SpecificationBuildRule(pbxspec::Manager::shared_ptr const &specManager, pbxspec::PBX::BuildRule::shared_ptr const &specBuildRule)
{
    std::string ts = specBuildRule->compilerSpec();
    pbxspec::PBX::Tool::shared_ptr tool = specManager->tool(ts) ?: std::static_pointer_cast<pbxspec::PBX::Tool>(specManager->compiler(ts)) ?: std::static_pointer_cast<pbxspec::PBX::Tool>(specManager->linker(ts));
    if (tool == nullptr) {
        return nullptr;
    }

    pbxspec::PBX::FileType::vector fileTypes;
    for (std::string const &ft : specBuildRule->fileTypes()) {
        pbxspec::PBX::FileType::shared_ptr fileType = specManager->fileType(ft);
        if (fileType == nullptr) {
            return nullptr;
        }
        fileTypes.push_back(fileType);
    }

    return std::make_shared <TargetBuildRules::BuildRule> (TargetBuildRules::BuildRule(
        std::string(),
        fileTypes,
        tool,
        std::string(),
        std::vector<pbxsetting::Value>()
    ));
}

TargetBuildRules TargetBuildRules::
Create(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::Target::shared_ptr const &target)
{
    TargetBuildRules::BuildRule::vector buildRules;

    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        pbxproj::PBX::NativeTarget::shared_ptr nativeTarget = std::static_pointer_cast <pbxproj::PBX::NativeTarget> (target);
        for (pbxproj::PBX::BuildRule::shared_ptr const &projBuildRule : nativeTarget->buildRules()) {
            if (TargetBuildRules::BuildRule::shared_ptr buildRule = ProjectBuildRule(specManager, projBuildRule)) {
                buildRules.push_back(buildRule);
            }
        }
    }

    for (pbxspec::PBX::BuildRule::shared_ptr const &specBuildRule : specManager->buildRules()) {
        if (TargetBuildRules::BuildRule::shared_ptr buildRule = SpecificationBuildRule(specManager, specBuildRule)) {
            buildRules.push_back(buildRule);
        }
    }

    for (pbxspec::PBX::BuildRule::shared_ptr const &specBuildRule : specManager->synthesizedBuildRules()) {
        if (BuildRule::shared_ptr buildRule = SpecificationBuildRule(specManager, specBuildRule)) {
            buildRules.push_back(buildRule);
        }
    }

    return TargetBuildRules(buildRules);
}
