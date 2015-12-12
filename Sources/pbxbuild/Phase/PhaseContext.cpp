/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/Tool/ClangResolver.h>
#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/Tool/InfoPlistResolver.h>
#include <pbxbuild/Tool/ScriptResolver.h>
#include <pbxbuild/Tool/TouchResolver.h>
#include <pbxbuild/Tool/ToolResolver.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/TargetBuildRules.h>
#include <libutil/FSUtil.h>

using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::ToolContext;
using pbxbuild::Tool::ClangResolver;
using pbxbuild::Tool::CopyResolver;
using pbxbuild::Tool::InfoPlistResolver;
using pbxbuild::Tool::ScriptResolver;
using pbxbuild::Tool::TouchResolver;
using pbxbuild::Tool::ToolResolver;
using libutil::FSUtil;

PhaseContext::
PhaseContext(ToolContext const &toolContext) :
    _toolContext(toolContext)
{
}

PhaseContext::
~PhaseContext()
{
}

ClangResolver const *PhaseContext::
clangResolver(PhaseEnvironment const &phaseEnvironment)
{
    if (_clangResolver == nullptr) {
        _clangResolver = ClangResolver::Create(phaseEnvironment);
    }

    return _clangResolver.get();
}

CopyResolver const *PhaseContext::
copyResolver(PhaseEnvironment const &phaseEnvironment)
{
    if (_copyResolver == nullptr) {
        _copyResolver = CopyResolver::Create(phaseEnvironment);
    }

    return _copyResolver.get();
}

InfoPlistResolver const *PhaseContext::
infoPlistResolver(PhaseEnvironment const &phaseEnvironment)
{
    if (_infoPlistResolver == nullptr) {
        _infoPlistResolver = InfoPlistResolver::Create(phaseEnvironment);
    }

    return _infoPlistResolver.get();
}

ScriptResolver const *PhaseContext::
scriptResolver(PhaseEnvironment const &phaseEnvironment)
{
    if (_scriptResolver == nullptr) {
        _scriptResolver = ScriptResolver::Create(phaseEnvironment);
    }

    return _scriptResolver.get();
}

TouchResolver const *PhaseContext::
touchResolver(PhaseEnvironment const &phaseEnvironment)
{
    if (_touchResolver == nullptr) {
        _touchResolver = TouchResolver::Create(phaseEnvironment);
    }

    return _touchResolver.get();
}

ToolResolver const *PhaseContext::
toolResolver(PhaseEnvironment const &phaseEnvironment, std::string const &identifier)
{
    if (_toolResolvers.find(identifier) == _toolResolvers.end()) {
        std::unique_ptr<ToolResolver> toolResolver = ToolResolver::Create(phaseEnvironment, identifier);
        if (toolResolver == nullptr) {
            return nullptr;
        }

        _toolResolvers.insert({ identifier, *toolResolver });
    }

    return &_toolResolvers.at(identifier);
}

bool PhaseContext::
resolveBuildFile(
    PhaseEnvironment const &phaseEnvironment,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
    pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
    TypeResolvedFile const &file,
    std::string const &outputDirectory)
{
    TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);
    if (buildRule == nullptr) {
        fprintf(stderr, "warning: no matching build rule for %s (type %s)\n", file.filePath().c_str(), file.fileType()->identifier().c_str());
        return true;
    }

    if (!buildRule->script().empty()) {
        if (ScriptResolver const *scriptResolver = this->scriptResolver(phaseEnvironment)) {
            _scriptResolver->resolve(&_toolContext, environment, file.filePath(), buildRule);
            return true;
        } else {
            return false;
        }
    } else if (pbxspec::PBX::Tool::shared_ptr const &tool = buildRule->tool()) {
        if (tool->identifier() == ClangResolver::ToolIdentifier()) {
            std::string outputBaseName;
            auto it = targetEnvironment.buildFileDisambiguation().find(buildFile);
            if (it != targetEnvironment.buildFileDisambiguation().end()) {
                outputBaseName = it->second;
            } else {
                outputBaseName = FSUtil::GetBaseNameWithoutExtension(file.filePath());
            }

            if (ClangResolver const *clangResolver = this->clangResolver(phaseEnvironment)) {
                _clangResolver->resolveSource(&_toolContext, environment, file, buildFile->compilerFlags(), outputDirectory, outputBaseName);
                return true;
            } else {
                return false;
            }
        } else if (tool->identifier() == CopyResolver::ToolIdentifier()) {
            std::string logMessageTitle;
            switch (buildPhase->type()) {
                case pbxproj::PBX::BuildPhase::kTypeHeaders:
                    logMessageTitle = "CpHeader";
                case pbxproj::PBX::BuildPhase::kTypeResources:
                    logMessageTitle = "CpResource";
                default:
                    logMessageTitle = "PBXCp";
            }

            if (CopyResolver const *copyResolver = this->copyResolver(phaseEnvironment)) {
                _copyResolver->resolve(&_toolContext, environment, file.filePath(), outputDirectory, logMessageTitle);
                return true;
            } else {
                return false;
            }
        } else {
            std::string outputPath = outputDirectory + "/" + FSUtil::GetBaseName(file.filePath());

            if (ToolResolver const *toolResolver = this->toolResolver(phaseEnvironment, tool->identifier())) {
                toolResolver->resolve(&_toolContext, environment, { file.filePath() }, { outputPath });
                return true;
            } else {
                return false;
            }
        }
    } else {
        assert(false && "build rule should have a script or a tool");
        return false;
    }
}

