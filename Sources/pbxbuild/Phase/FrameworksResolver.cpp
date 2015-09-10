// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/LinkerInvocationContext.h>

using pbxbuild::Phase::FrameworksResolver;
using pbxbuild::Phase::PhaseContext;
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

std::unique_ptr<FrameworksResolver> FrameworksResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::FrameworksBuildPhase::shared_ptr const &buildPhase,
    std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> const &sourcesInvocations
)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseContext.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseContext.targetEnvironment();

    std::vector<pbxbuild::ToolInvocation> invocations;

    pbxspec::PBX::Linker::shared_ptr ld = buildEnvironment.specManager()->linker("com.apple.pbx.linkers.ld", targetEnvironment.specDomain());
    pbxspec::PBX::Linker::shared_ptr libtool = buildEnvironment.specManager()->linker("com.apple.pbx.linkers.libtool", targetEnvironment.specDomain());
    pbxspec::PBX::Linker::shared_ptr lipo = buildEnvironment.specManager()->linker("com.apple.xcode.linkers.lipo", targetEnvironment.specDomain());
    pbxspec::PBX::Tool::shared_ptr dsymutil = buildEnvironment.specManager()->tool("com.apple.tools.dsymutil", targetEnvironment.specDomain());
    if (ld == nullptr || libtool == nullptr || lipo == nullptr || dsymutil == nullptr) {
        fprintf(stderr, "error: couldn't get linker tools\n");
        return nullptr;
    }

    std::string binaryType = targetEnvironment.environment().resolve("MACH_O_TYPE");

    pbxspec::PBX::Linker::shared_ptr linker;
    std::string linkerExecutable;
    if (binaryType == "staticlib") {
        linker = libtool;
    } else {
        linker = ld;
        // TODO(grp): Resolve this from the Compiler used to build sources? (See ExecCPlusPlusLinkerPath.)
        linkerExecutable = "clang";
    }

    std::string workingDirectory = targetEnvironment.workingDirectory();
    std::string productsDirectory = targetEnvironment.environment().resolve("BUILT_PRODUCTS_DIR");

    for (std::string const &variant : targetEnvironment.variants()) {
        pbxsetting::Environment variantEnvironment = targetEnvironment.environment();
        variantEnvironment.insertFront(PhaseContext::VariantLevel(variant), false);

        std::string variantIntermediatesName = variantEnvironment.resolve("EXECUTABLE_NAME") + variantEnvironment.resolve("EXECUTABLE_VARIANT_SUFFIX");
        std::string variantIntermediatesDirectory = variantEnvironment.resolve("OBJECT_FILE_DIR_" + variant);

        std::string variantProductsPath = variantEnvironment.resolve("EXECUTABLE_PATH") + variantEnvironment.resolve("EXECUTABLE_VARIANT_SUFFIX");
        std::string variantProductsOutput = productsDirectory + "/" + variantProductsPath;

        bool createUniversalBinary = targetEnvironment.architectures().size() > 1;
        std::vector<std::string> universalBinaryInputs;

        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment archEnvironment = variantEnvironment;
            archEnvironment.insertFront(PhaseContext::ArchitectureLevel(arch), false);

            auto buildFiles = phaseContext.resolveBuildFiles(archEnvironment, buildPhase->files());

            std::vector<pbxbuild::TypeResolvedFile> files;
            files.reserve(buildFiles.size());
            for (auto const &entry : buildFiles) {
                files.push_back(entry.second);
            }

            std::vector<std::string> sourceOutputs;
            auto it = sourcesInvocations.find(std::make_pair(variant, arch));
            if (it != sourcesInvocations.end()) {
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

                auto context = pbxbuild::LinkerInvocationContext::Create(linker, sourceOutputs, files, architectureIntermediatesOutput, archEnvironment, workingDirectory, linkerExecutable);
                invocations.push_back(context.invocation());

                universalBinaryInputs.push_back(architectureIntermediatesOutput);
            } else {
                auto context = pbxbuild::LinkerInvocationContext::Create(linker, sourceOutputs, files, variantProductsOutput, archEnvironment, workingDirectory, linkerExecutable);
                invocations.push_back(context.invocation());
            }
        }

        if (createUniversalBinary) {
            auto context = pbxbuild::LinkerInvocationContext::Create(lipo, universalBinaryInputs, { }, variantProductsOutput, variantEnvironment, workingDirectory);
            invocations.push_back(context.invocation());
        }

        if (variantEnvironment.resolve("DEBUG_INFORMATION_FORMAT") == "dwarf-with-dsym" && (binaryType != "staticlib" && binaryType != "mh_object")) {
            std::string dsymfile = variantEnvironment.resolve("DWARF_DSYM_FOLDER_PATH") + "/" + variantEnvironment.resolve("DWARF_DSYM_FILE_NAME");
            auto context = pbxbuild::ToolInvocationContext::Create(dsymutil, { variantProductsOutput }, { dsymfile }, variantEnvironment, workingDirectory);
            invocations.push_back(context.invocation());
        }
    }

    return std::make_unique<FrameworksResolver>(invocations);
}
