/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/Tool/ToolResolver.h>
#include <pbxbuild/Tool/LinkerResolver.h>

using pbxbuild::Phase::FrameworksResolver;
using pbxbuild::Phase::PhaseEnvironment;
using pbxbuild::Phase::SourcesResolver;
using pbxbuild::Tool::LinkerResolver;
using pbxbuild::Tool::ToolResolver;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

FrameworksResolver::
FrameworksResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

FrameworksResolver::
~FrameworksResolver()
{
}

static std::vector<pbxbuild::TypeResolvedFile>
InputFiles(pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment, pbxproj::PBX::FrameworksBuildPhase::shared_ptr const &buildPhase, pbxsetting::Environment const &environment)
{
    std::vector<pbxbuild::TypeResolvedFile> files;

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
        if (buildFile->fileRef() == nullptr) {
            continue;
        }

        std::unique_ptr<TypeResolvedFile> file = nullptr;

        switch (buildFile->fileRef()->type()) {
            case pbxproj::PBX::GroupItem::kTypeFileReference: {
                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                file = phaseEnvironment.resolveFileReference(fileReference, environment);
                break;
            }
            case pbxproj::PBX::GroupItem::kTypeReferenceProxy: {
                pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy = std::static_pointer_cast <pbxproj::PBX::ReferenceProxy> (buildFile->fileRef());
                file = phaseEnvironment.resolveReferenceProxy(referenceProxy, environment);
                break;
            }
            default: {
                fprintf(stderr, "warning: unsupported build file for frameworks build phase\n");
                break;
            }
        }

        if (file != nullptr) {
            files.push_back(*file);
        }
    }

    return files;
}

std::unique_ptr<FrameworksResolver> FrameworksResolver::
Create(
    pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment,
    pbxproj::PBX::FrameworksBuildPhase::shared_ptr const &buildPhase,
    SourcesResolver const &sourcesResolver
)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    std::vector<pbxbuild::ToolInvocation> invocations;

    std::unique_ptr<LinkerResolver> ldResolver = LinkerResolver::Create(phaseEnvironment, LinkerResolver::LinkerToolIdentifier());
    std::unique_ptr<LinkerResolver> libtoolResolver = LinkerResolver::Create(phaseEnvironment, LinkerResolver::LibtoolToolIdentifier());
    std::unique_ptr<LinkerResolver> lipoResolver = LinkerResolver::Create(phaseEnvironment, LinkerResolver::LipoToolIdentifier());
    std::unique_ptr<ToolResolver> dsymutil = ToolResolver::Create(phaseEnvironment, "com.apple.tools.dsymutil");
    if (ldResolver == nullptr || libtoolResolver == nullptr || lipoResolver == nullptr || dsymutil == nullptr) {
        fprintf(stderr, "error: couldn't get linker tools\n");
        return nullptr;
    }

    std::string binaryType = targetEnvironment.environment().resolve("MACH_O_TYPE");

    LinkerResolver *linkerResolver = nullptr;
    std::string linkerExecutable;
    std::vector<std::string> linkerArguments;

    if (binaryType == "staticlib") {
        linkerResolver = libtoolResolver.get();
    } else {
        linkerResolver = ldResolver.get();
        linkerExecutable = sourcesResolver.linkerDriver();
        linkerArguments.insert(linkerArguments.end(), sourcesResolver.linkerArgs().begin(), sourcesResolver.linkerArgs().end());
    }

    std::string workingDirectory = targetEnvironment.workingDirectory();
    std::string productsDirectory = targetEnvironment.environment().resolve("BUILT_PRODUCTS_DIR");

    std::vector<pbxbuild::TypeResolvedFile> files = InputFiles(phaseEnvironment, buildPhase, targetEnvironment.environment());

    for (std::string const &variant : targetEnvironment.variants()) {
        pbxsetting::Environment variantEnvironment = targetEnvironment.environment();
        variantEnvironment.insertFront(PhaseEnvironment::VariantLevel(variant), false);

        std::string variantIntermediatesName = variantEnvironment.resolve("EXECUTABLE_NAME") + variantEnvironment.resolve("EXECUTABLE_VARIANT_SUFFIX");
        std::string variantIntermediatesDirectory = variantEnvironment.resolve("OBJECT_FILE_DIR_" + variant);

        std::string variantProductsPath = variantEnvironment.resolve("EXECUTABLE_PATH") + variantEnvironment.resolve("EXECUTABLE_VARIANT_SUFFIX");
        std::string variantProductsOutput = productsDirectory + "/" + variantProductsPath;

        bool createUniversalBinary = targetEnvironment.architectures().size() > 1;
        std::vector<std::string> universalBinaryInputs;

        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment archEnvironment = variantEnvironment;
            archEnvironment.insertFront(PhaseEnvironment::ArchitectureLevel(arch), false);

            std::vector<std::string> sourceOutputs;
            auto it = sourcesResolver.variantArchitectureInvocations().find(std::make_pair(variant, arch));
            if (it != sourcesResolver.variantArchitectureInvocations().end()) {
                std::vector<pbxbuild::ToolInvocation> const &sourceInvocations = it->second;
                for (pbxbuild::ToolInvocation const &invocation : sourceInvocations) {
                    for (std::string const &output : invocation.outputs()) {
                        // TODO(grp): Is this the right set of source outputs to link?
                        if (libutil::FSUtil::GetFileExtension(output) == "o") {
                            sourceOutputs.push_back(output);
                        }
                    }
                }
            }

            if (createUniversalBinary) {
                std::string architectureIntermediatesDirectory = variantIntermediatesDirectory + "/" + arch;
                std::string architectureIntermediatesOutput = architectureIntermediatesDirectory + "/" + variantIntermediatesName;

                ToolInvocation invocation = linkerResolver->invocation(sourceOutputs, files, architectureIntermediatesOutput, linkerArguments, archEnvironment, workingDirectory, linkerExecutable);
                invocations.push_back(invocation);

                universalBinaryInputs.push_back(architectureIntermediatesOutput);
            } else {
                ToolInvocation invocation = linkerResolver->invocation(sourceOutputs, files, variantProductsOutput, linkerArguments, archEnvironment, workingDirectory, linkerExecutable);
                invocations.push_back(invocation);
            }
        }

        if (createUniversalBinary) {
            ToolInvocation invocation = lipoResolver->invocation(universalBinaryInputs, { }, variantProductsOutput, { }, variantEnvironment, workingDirectory);
            invocations.push_back(invocation);
        }

        if (variantEnvironment.resolve("DEBUG_INFORMATION_FORMAT") == "dwarf-with-dsym" && (binaryType != "staticlib" && binaryType != "mh_object")) {
            std::string dsymfile = variantEnvironment.resolve("DWARF_DSYM_FOLDER_PATH") + "/" + variantEnvironment.resolve("DWARF_DSYM_FILE_NAME");
            ToolInvocation invocation = dsymutil->invocation({ variantProductsOutput }, { dsymfile }, variantEnvironment, workingDirectory);
            invocations.push_back(invocation);
        }
    }

    return std::unique_ptr<FrameworksResolver>(new FrameworksResolver(invocations));
}
