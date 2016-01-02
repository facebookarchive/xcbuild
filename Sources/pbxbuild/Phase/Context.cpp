/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/Tool/ClangResolver.h>
#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/Tool/InfoPlistResolver.h>
#include <pbxbuild/Tool/MakeDirectoryResolver.h>
#include <pbxbuild/Tool/ScriptResolver.h>
#include <pbxbuild/Tool/SymlinkResolver.h>
#include <pbxbuild/Tool/TouchResolver.h>
#include <pbxbuild/Tool/ToolResolver.h>
#include <pbxbuild/Target/Environment.h>
#include <pbxbuild/Target/BuildRules.h>
#include <libutil/FSUtil.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;
namespace Target = pbxbuild::Target;
using libutil::FSUtil;

Phase::Context::
Context(Tool::Context const &toolContext) :
    _toolContext(toolContext)
{
}

Phase::Context::
~Context()
{
}

Tool::ClangResolver const *Phase::Context::
clangResolver(Phase::Environment const &phaseEnvironment)
{
    if (_clangResolver == nullptr) {
        _clangResolver = Tool::ClangResolver::Create(phaseEnvironment);
    }

    return _clangResolver.get();
}

Tool::CopyResolver const *Phase::Context::
copyResolver(Phase::Environment const &phaseEnvironment)
{
    if (_copyResolver == nullptr) {
        _copyResolver = Tool::CopyResolver::Create(phaseEnvironment);
    }

    return _copyResolver.get();
}

Tool::InfoPlistResolver const *Phase::Context::
infoPlistResolver(Phase::Environment const &phaseEnvironment)
{
    if (_infoPlistResolver == nullptr) {
        _infoPlistResolver = Tool::InfoPlistResolver::Create(phaseEnvironment);
    }

    return _infoPlistResolver.get();
}

Tool::MakeDirectoryResolver const *Phase::Context::
makeDirectoryResolver(Phase::Environment const &phaseEnvironment)
{
    if (_makeDirectoryResolver == nullptr) {
        _makeDirectoryResolver = Tool::MakeDirectoryResolver::Create(phaseEnvironment);
    }

    return _makeDirectoryResolver.get();
}

Tool::ScriptResolver const *Phase::Context::
scriptResolver(Phase::Environment const &phaseEnvironment)
{
    if (_scriptResolver == nullptr) {
        _scriptResolver = Tool::ScriptResolver::Create(phaseEnvironment);
    }

    return _scriptResolver.get();
}

Tool::SymlinkResolver const *Phase::Context::
symlinkResolver(Phase::Environment const &phaseEnvironment)
{
    if (_symlinkResolver == nullptr) {
        _symlinkResolver = Tool::SymlinkResolver::Create(phaseEnvironment);
    }

    return _symlinkResolver.get();
}

Tool::TouchResolver const *Phase::Context::
touchResolver(Phase::Environment const &phaseEnvironment)
{
    if (_touchResolver == nullptr) {
        _touchResolver = Tool::TouchResolver::Create(phaseEnvironment);
    }

    return _touchResolver.get();
}

Tool::ToolResolver const *Phase::Context::
toolResolver(Phase::Environment const &phaseEnvironment, std::string const &identifier)
{
    if (_toolResolvers.find(identifier) == _toolResolvers.end()) {
        std::unique_ptr<Tool::ToolResolver> toolResolver = Tool::ToolResolver::Create(phaseEnvironment, identifier);
        if (toolResolver == nullptr) {
            return nullptr;
        }

        _toolResolvers.insert({ identifier, *toolResolver });
    }

    return &_toolResolvers.at(identifier);
}

bool Phase::Context::
resolveBuildFiles(
    Phase::Environment const &phaseEnvironment,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
    std::string const &outputDirectory,
    std::vector<Phase::File> const &files,
    std::string const &fallbackToolIdentifier)
{
    for (Phase::File const &file : files) {
        std::string fileOutputDirectory = outputDirectory;
        if (!file.outputSubdirectory().empty()) {
            fileOutputDirectory += "/" + file.outputSubdirectory();
        }

        bool result = resolveBuildFile(
            phaseEnvironment,
            environment,
            buildPhase,
            fileOutputDirectory,
            file,
            fallbackToolIdentifier);
        if (!result) {
            return false;
        }
    }

    return true;
}

bool Phase::Context::
resolveBuildFile(
    Phase::Environment const &phaseEnvironment,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
    std::string const &outputDirectory,
    Phase::File const &file,
    std::string const &fallbackToolIdentifier)
{
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    Target::BuildRules::BuildRule::shared_ptr const &buildRule = file.buildRule();
    if (buildRule == nullptr && fallbackToolIdentifier.empty()) {
        fprintf(stderr, "warning: no matching build rule for %s (type %s)\n", file.path().c_str(), file.fileType()->identifier().c_str());
        return true;
    }

    if (buildRule != nullptr && !buildRule->script().empty()) {
        if (Tool::ScriptResolver const *scriptResolver = this->scriptResolver(phaseEnvironment)) {
            scriptResolver->resolve(&_toolContext, environment, file.path(), buildRule);
            return true;
        } else {
            return false;
        }
    } else {
        std::string toolIdentifier = fallbackToolIdentifier;

        if (buildRule != nullptr) {
            if (pbxspec::PBX::Tool::shared_ptr const &tool = buildRule->tool()) {
                toolIdentifier = tool->identifier();
            }
        }

        if (toolIdentifier.empty()) {
            fprintf(stderr, "warning: no tool available for build rule\n");
            return false;
        }

        if (toolIdentifier == Tool::ClangResolver::ToolIdentifier()) {
            if (Tool::ClangResolver const *clangResolver = this->clangResolver(phaseEnvironment)) {
                clangResolver->resolveSource(&_toolContext, environment, file, outputDirectory);
                return true;
            } else {
                return false;
            }
        } else if (toolIdentifier == Tool::CopyResolver::ToolIdentifier()) {
            std::string logMessageTitle;
            switch (buildPhase->type()) {
                case pbxproj::PBX::BuildPhase::kTypeHeaders:
                    logMessageTitle = "CpHeader";
                case pbxproj::PBX::BuildPhase::kTypeResources:
                    logMessageTitle = "CpResource";
                default:
                    logMessageTitle = "PBXCp";
            }

            if (Tool::CopyResolver const *copyResolver = this->copyResolver(phaseEnvironment)) {
                copyResolver->resolve(&_toolContext, environment, file.path(), outputDirectory, logMessageTitle);
                return true;
            } else {
                return false;
            }
        } else {
            std::string outputPath = outputDirectory + "/" + FSUtil::GetBaseName(file.path());

            if (Tool::ToolResolver const *toolResolver = this->toolResolver(phaseEnvironment, toolIdentifier)) {
                toolResolver->resolve(&_toolContext, environment, { file.path() }, { outputPath });
                return true;
            } else {
                return false;
            }
        }
    }
}

