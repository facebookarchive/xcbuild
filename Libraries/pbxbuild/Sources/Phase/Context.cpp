/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/Tool/AssetCatalogResolver.h>
#include <pbxbuild/Tool/ClangResolver.h>
#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/Tool/DittoResolver.h>
#include <pbxbuild/Tool/InfoPlistResolver.h>
#include <pbxbuild/Tool/InterfaceBuilderResolver.h>
#include <pbxbuild/Tool/MakeDirectoryResolver.h>
#include <pbxbuild/Tool/ScriptResolver.h>
#include <pbxbuild/Tool/SwiftResolver.h>
#include <pbxbuild/Tool/SymlinkResolver.h>
#include <pbxbuild/Tool/TouchResolver.h>
#include <pbxbuild/Tool/ToolResolver.h>
#include <pbxbuild/Target/Environment.h>
#include <pbxbuild/Target/BuildRules.h>
#include <libutil/FSUtil.h>

#include <cassert>

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

Tool::AssetCatalogResolver const *Phase::Context::
assetCatalogResolver(Phase::Environment const &phaseEnvironment)
{
    if (_assetCatalogResolver == nullptr) {
        _assetCatalogResolver = Tool::AssetCatalogResolver::Create(phaseEnvironment);
    }

    return _assetCatalogResolver.get();
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

Tool::DittoResolver const *Phase::Context::
dittoResolver(Phase::Environment const &phaseEnvironment)
{
    if (_dittoResolver == nullptr) {
        _dittoResolver = Tool::DittoResolver::Create(phaseEnvironment);
    }

    return _dittoResolver.get();
}

Tool::InfoPlistResolver const *Phase::Context::
infoPlistResolver(Phase::Environment const &phaseEnvironment)
{
    if (_infoPlistResolver == nullptr) {
        _infoPlistResolver = Tool::InfoPlistResolver::Create(phaseEnvironment);
    }

    return _infoPlistResolver.get();
}

Tool::InterfaceBuilderResolver const *Phase::Context::
interfaceBuilderCompilerResolver(Phase::Environment const &phaseEnvironment)
{
    if (_interfaceBuilderCompilerResolver == nullptr) {
        _interfaceBuilderCompilerResolver = Tool::InterfaceBuilderResolver::Create(phaseEnvironment, Tool::InterfaceBuilderResolver::CompilerToolIdentifier());
    }

    return _interfaceBuilderCompilerResolver.get();
}

Tool::InterfaceBuilderResolver const *Phase::Context::
interfaceBuilderStoryboardCompilerResolver(Phase::Environment const &phaseEnvironment)
{
    if (_interfaceBuilderStoryboardCompilerResolver == nullptr) {
        _interfaceBuilderStoryboardCompilerResolver = Tool::InterfaceBuilderResolver::Create(phaseEnvironment, Tool::InterfaceBuilderResolver::StoryboardCompilerToolIdentifier());
    }

    return _interfaceBuilderStoryboardCompilerResolver.get();
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

Tool::SwiftResolver const *Phase::Context::
swiftResolver(Phase::Environment const &phaseEnvironment)
{
    if (_swiftResolver == nullptr) {
        _swiftResolver = Tool::SwiftResolver::Create(phaseEnvironment);
    }

    return _swiftResolver.get();
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

std::vector<std::vector<Phase::File>> Phase::Context::
Group(std::vector<Phase::File> const &files)
{
    std::vector<Phase::File> ungrouped;
    std::unordered_map<std::string, std::vector<Phase::File>> groupedTool;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Phase::File>>> groupedCommonBase;
    std::vector<Phase::File> groupedBaseRegion;

    /*
     * Determine which grouping method to use for each file.
     */
    for (Phase::File const &file : files) {
        /* Get the tool used for the file. If null, then no grouping possible. */
        Target::BuildRules::BuildRule::shared_ptr const &buildRule = file.buildRule();
        if (buildRule == nullptr || buildRule->tool() == nullptr || buildRule->tool()->type() != pbxspec::PBX::Compiler::Type()) {
            ungrouped.push_back(file);
            continue;
        }
        pbxspec::PBX::Compiler::shared_ptr const &compiler = std::static_pointer_cast<pbxspec::PBX::Compiler>(buildRule->tool());

        /* Determine the grouping. Only a single grouping per file is supported. */
        std::vector<std::string> const &groupings = compiler->inputFileGroupings().value_or(std::vector<std::string>());
        if (groupings.size() != 1) {
            if (!groupings.empty()) {
                fprintf(stderr, "error: more than one input file grouping is not supported\n");
            }
            ungrouped.push_back(file);
            continue;
        }

        std::string const &grouping = groupings.front();

        if (grouping == "tool") {
            /* Tool groupings are keyed on just the tool. */
            groupedTool[compiler->identifier()].push_back(file);
        } else if (grouping == "common-file-base") {
            /* Keyed on both the file name and tool. */
            std::string base = FSUtil::GetBaseNameWithoutExtension(file.path());
            groupedCommonBase[compiler->identifier()][base].push_back(file);
        } else if (grouping == "ib-base-region-and-strings") {
            /* Only "Base" region files. See below for finding additional grouped files. */
            if (file.localization() == "Base") {
                groupedBaseRegion.push_back(file);
            } else {
                ungrouped.push_back(file);
            }
        } else {
            fprintf(stderr, "error: unknown grouping '%s'\n", grouping.c_str());
            ungrouped.push_back(file);
        }
    }

    /*
     * Build up the result, a list of each set of grouped inputs.
     */
    std::vector<std::vector<Phase::File>> result;

    /*
     * Add tool groupings to the result.
     */
    for (auto const &entry : groupedTool) {
        result.push_back(entry.second);
    }

    /*
     * Add common base name groupings to the result.
     */
    for (auto const &entry1 : groupedCommonBase) {
        for (auto const &entry2 : entry1.second) {
            result.push_back(entry2.second);
        }
    }

    /*
     * Add base region groupings to the result.
     */
    for (Phase::File const &file : groupedBaseRegion) {
        std::vector<Phase::File> inputs = { file };

        /*
         * Find all .strings files from the same build file (i.e. same variant group)
         * as the base. Add them to the inputs for this group, and remove them from
         * the ungrouped  inputs so they don't get added again to the result.
         */
        ungrouped.erase(std::remove_if(ungrouped.begin(), ungrouped.end(), [&](Phase::File const &ungroupedFile) {
            if (ungroupedFile.fileType()->identifier() == "text.plist.strings") {
                if (ungroupedFile.buildFile() == file.buildFile()) {
                    inputs.push_back(ungroupedFile);
                    return true;
                }
            }

            return false;
        }), ungrouped.end());

        result.push_back(inputs);
    }

    /*
     * Add ungrouped files to the result, one grouping per file. Note this must come
     * after the base region grouping above as the base region modifies the ungrouped.
     */
    for (Phase::File const &file : ungrouped) {
        result.push_back({ file });
    }

    return result;
}

bool Phase::Context::
resolveBuildFiles(
    Phase::Environment const &phaseEnvironment,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
    std::vector<std::vector<Phase::File>> const &groups,
    std::string const &outputDirectory,
    std::string const &fallbackToolIdentifier)
{
    for (std::vector<Phase::File> const &files : groups) {
        assert(!files.empty());
        Phase::File const &first = files.front();

        std::string fileOutputDirectory = outputDirectory;
        if (!first.localization().empty()) {
            fileOutputDirectory += "/" + first.localization() + ".lproj";
        }

        Target::BuildRules::BuildRule::shared_ptr const &buildRule = first.buildRule();
        if (buildRule == nullptr && fallbackToolIdentifier.empty()) {
            fprintf(stderr, "warning: no matching build rule for %s (type %s)\n", first.path().c_str(), first.fileType()->identifier().c_str());
            continue;
        }

        if (buildRule != nullptr && !buildRule->script().empty()) {
            if (Tool::ScriptResolver const *scriptResolver = this->scriptResolver(phaseEnvironment)) {
                assert(files.size() == 1); // TODO(grp): Is this a valid assertion?
                scriptResolver->resolve(&_toolContext, environment, first);
            } else {
                return false;
            }
        } else {
            std::string toolIdentifier = fallbackToolIdentifier;

            if (buildRule != nullptr) {
                if (pbxspec::PBX::Tool::shared_ptr const &tool = buildRule->tool()) {
                    // Some tools additionally limit their file types beyond what their build rule allows.
                    // For example, the default compiler limits itself to just source files, despite its
                    // default build rule specifying that it accepts all C-family inputs, including headers.
                    // TODO(grp): Is this the right way to make .h files not get compiled as resources?
                    if (tool->fileTypes() || tool->inputFileTypes()) {
                        std::vector<std::string> toolFileTypes;
                        if (tool->fileTypes()) {
                            toolFileTypes.insert(toolFileTypes.end(), tool->fileTypes()->begin(), tool->fileTypes()->end());
                        }
                        if (tool->inputFileTypes()) {
                            toolFileTypes.insert(toolFileTypes.end(), tool->inputFileTypes()->begin(), tool->inputFileTypes()->end());
                        }

                        std::string inputFileType = first.fileType()->identifier();
                        bool toolAcceptsInputFileType = (toolFileTypes.empty() || std::find(toolFileTypes.begin(), toolFileTypes.end(), inputFileType) != toolFileTypes.end());

                        if (toolAcceptsInputFileType) {
                            toolIdentifier = tool->identifier();
                        }
                    } else {
                        toolIdentifier = tool->identifier();
                    }
                }
            }

            if (toolIdentifier.empty()) {
                fprintf(stderr, "warning: no tool available for build rule\n");
                return false;
            }

            if (toolIdentifier == Tool::AssetCatalogResolver::ToolIdentifier()) {
                if (Tool::AssetCatalogResolver const *assetCatalogResolver = this->assetCatalogResolver(phaseEnvironment)) {
                    assetCatalogResolver->resolve(&_toolContext, environment, files);
                } else {
                    return false;
                }
            } else if (toolIdentifier == Tool::ClangResolver::ToolIdentifier()) {
                if (Tool::ClangResolver const *clangResolver = this->clangResolver(phaseEnvironment)) {
                    assert(files.size() == 1); // TODO(grp): Is this a valid assertion?
                    clangResolver->resolveSource(&_toolContext, environment, first, fileOutputDirectory);
                } else {
                    return false;
                }
            } else if (toolIdentifier == Tool::CopyResolver::ToolIdentifier()) {
                std::string logMessageTitle;
                switch (buildPhase->type()) {
                    case pbxproj::PBX::BuildPhase::Type::Headers:
                        logMessageTitle = "CpHeader";
                    case pbxproj::PBX::BuildPhase::Type::Resources:
                        logMessageTitle = "CpResource";
                    default:
                        logMessageTitle = "PBXCp";
                }

                if (Tool::CopyResolver const *copyResolver = this->copyResolver(phaseEnvironment)) {
                    copyResolver->resolve(&_toolContext, environment, files, fileOutputDirectory, logMessageTitle);
                } else {
                    return false;
                }
            } else if (toolIdentifier == Tool::InterfaceBuilderResolver::CompilerToolIdentifier()) {
                if (Tool::InterfaceBuilderResolver const *interfaceBuilderCompilerResolver = this->interfaceBuilderCompilerResolver(phaseEnvironment)) {
                    interfaceBuilderCompilerResolver->resolve(&_toolContext, environment, files);
                } else {
                    return false;
                }
            } else if (toolIdentifier == Tool::InterfaceBuilderResolver::StoryboardCompilerToolIdentifier()) {
                if (Tool::InterfaceBuilderResolver const *interfaceBuilderStoryboardCompilerResolver = this->interfaceBuilderStoryboardCompilerResolver(phaseEnvironment)) {
                    interfaceBuilderStoryboardCompilerResolver->resolve(&_toolContext, environment, files);
                } else {
                    return false;
                }
            } else if (toolIdentifier == Tool::SwiftResolver::ToolIdentifier()) {
                if (Tool::SwiftResolver const *swiftResolver = this->swiftResolver(phaseEnvironment)) {
                    swiftResolver->resolve(&_toolContext, environment, files, fileOutputDirectory);
                } else {
                    return false;
                }
            } else {
                if (Tool::ToolResolver const *toolResolver = this->toolResolver(phaseEnvironment, toolIdentifier)) {
                    toolResolver->resolve(&_toolContext, environment, files, fileOutputDirectory);
                } else {
                    return false;
                }
            }
        }
    }

    return true;
}

