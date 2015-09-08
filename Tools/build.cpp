// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <xcscheme/xcscheme.h>
#include <xcworkspace/xcworkspace.h>
#include <pbxbuild/pbxbuild.h>

using libutil::FSUtil;

static std::unique_ptr<pbxbuild::FileTypeResolver>
ResolveBuildFile(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxsetting::Environment const &environment, pbxproj::PBX::BuildFile::shared_ptr const &buildFile)
{
    if (pbxproj::PBX::FileReference::shared_ptr const &fileReference = buildFile->fileReference()) {
        std::string path = environment.expand(fileReference->resolve());
        return pbxbuild::FileTypeResolver::Resolve(buildEnvironment.specManager(), fileReference, environment);
    } else if (pbxproj::PBX::ReferenceProxy::shared_ptr referenceProxy = buildFile->referenceProxy()) {
        pbxproj::PBX::ContainerItemProxy::shared_ptr proxy = referenceProxy->remoteRef();
        pbxproj::PBX::FileReference::shared_ptr containerReference = proxy->containerPortal();
        std::string containerPath = environment.expand(containerReference->resolve());

        auto remote = buildContext.resolveProductIdentifier(buildContext.project(containerPath), proxy->remoteGlobalIDString());
        if (remote == nullptr) {
            fprintf(stderr, "error: unable to find remote target product from proxied reference\n");
            return nullptr;
        }

        std::unique_ptr<pbxbuild::TargetEnvironment> remoteEnvironment = buildContext.targetEnvironment(buildEnvironment, remote->first);
        if (remoteEnvironment == nullptr) {
            fprintf(stderr, "error: unable to create target environment for remote target\n");
            return nullptr;
        }

        return pbxbuild::FileTypeResolver::Resolve(buildEnvironment.specManager(), remote->second, remoteEnvironment->environment());
    } else {
        fprintf(stderr, "error: unable to handle build file without file reference or proxy\n");
        return nullptr;
    }
}

static std::vector<pbxbuild::FileTypeResolver>
ResolveBuildFiles(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxsetting::Environment const &environment, std::vector<pbxproj::PBX::BuildFile::shared_ptr> const &buildFiles)
{
    std::vector<pbxbuild::FileTypeResolver> files;
    files.reserve(buildFiles.size());

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildFiles) {
        auto file = ResolveBuildFile(buildEnvironment, buildContext, environment, buildFile);
        if (file == nullptr) {
            continue;
        }
        files.push_back(*file);
    }

    return files;
}

static pbxsetting::Level
VariantLevel(std::string const &variant)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_VARIANT", variant),
        pbxsetting::Setting::Parse("variant", variant),
        pbxsetting::Setting::Parse("EXECUTABLE_VARIANT_SUFFIX", variant != "normal" ? "_" + variant : ""),
    });
}

static pbxsetting::Level
ArchitectureLevel(std::string const &arch)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_ARCH", arch),
        pbxsetting::Setting::Parse("arch", arch),
    });
}

static std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>>
CompileFiles(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxbuild::TargetEnvironment const &targetEnvironment, pbxproj::PBX::SourcesBuildPhase::shared_ptr const &buildPhase)
{
    std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> result;

    pbxspec::PBX::Tool::shared_ptr scriptTool = buildEnvironment.specManager()->tool("com.apple.commands.shell-script", targetEnvironment.specDomain());
    if (scriptTool == nullptr) {
        return result;
    }

    std::string workingDirectory = targetEnvironment.workingDirectory();

    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment currentEnvironment = targetEnvironment.environment();
            currentEnvironment.insertFront(VariantLevel(variant));
            currentEnvironment.insertFront(ArchitectureLevel(arch));

            std::vector<pbxbuild::ToolInvocation> invocations;

            std::vector<pbxbuild::FileTypeResolver> files = ResolveBuildFiles(buildEnvironment, buildContext, currentEnvironment, buildPhase->files());
            for (pbxbuild::FileTypeResolver const &file : files) {
                pbxbuild::TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);

                if (buildRule != nullptr) {
                    if (buildRule->tool() != nullptr) {
                        pbxspec::PBX::Tool::shared_ptr tool = buildRule->tool();
                        if (tool->identifier() == "com.apple.compilers.gcc") {
                            std::string gccVersion = currentEnvironment.resolve("GCC_VERSION");
                            // TODO(grp): This should probably try a number of other compilers if it's not clang.
                            pbxspec::PBX::Compiler::shared_ptr compiler = buildEnvironment.specManager()->compiler(gccVersion + ".compiler", targetEnvironment.specDomain());

                            auto context = pbxbuild::CompilerInvocationContext::Create(compiler, file, currentEnvironment, workingDirectory);
                            invocations.push_back(context.invocation());
                        } else {
                            // TODO(grp): Use an appropriate compiler context to create this invocation.
                            auto context = pbxbuild::ToolInvocationContext::Create(tool, { }, { file.filePath() }, currentEnvironment, workingDirectory);
                            invocations.push_back(context.invocation());
                        }
                    } else if (!buildRule->script().empty()) {
                        auto context = pbxbuild::ScriptInvocationContext::Create(scriptTool, file.filePath(), buildRule, currentEnvironment, workingDirectory);
                        invocations.push_back(context.invocation());
                    }
                } else {
                    fprintf(stderr, "warning: no matching build rule for %s (type %s)\n", file.filePath().c_str(), file.fileType()->identifier().c_str());
                    continue;
                }
            }

            std::pair<std::string, std::string> resultKey = std::make_pair(variant, arch);
            result.insert({ resultKey, invocations });
        }
    }

    return result;
}

static std::vector<pbxbuild::ToolInvocation>
LinkFiles(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxbuild::TargetEnvironment const &targetEnvironment, pbxproj::PBX::FrameworksBuildPhase::shared_ptr const &buildPhase, std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> const &sourcesInvocations)
{
    std::vector<pbxbuild::ToolInvocation> invocations;

    pbxspec::PBX::Linker::shared_ptr ld = buildEnvironment.specManager()->linker("com.apple.pbx.linkers.ld", targetEnvironment.specDomain());
    pbxspec::PBX::Linker::shared_ptr libtool = buildEnvironment.specManager()->linker("com.apple.pbx.linkers.libtool", targetEnvironment.specDomain());
    pbxspec::PBX::Linker::shared_ptr lipo = buildEnvironment.specManager()->linker("com.apple.xcode.linkers.lipo", targetEnvironment.specDomain());
    pbxspec::PBX::Tool::shared_ptr dsymutil = buildEnvironment.specManager()->tool("com.apple.tools.dsymutil", targetEnvironment.specDomain());
    if (ld == nullptr || libtool == nullptr || lipo == nullptr || dsymutil == nullptr) {
        fprintf(stderr, "error: couldn't get linker tools\n");
        return invocations;
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
        variantEnvironment.insertFront(VariantLevel(variant));

        std::string variantIntermediatesName = variantEnvironment.resolve("EXECUTABLE_NAME") + variantEnvironment.resolve("EXECUTABLE_VARIANT_SUFFIX");
        std::string variantIntermediatesDirectory = variantEnvironment.resolve("OBJECT_FILE_DIR_" + variant);

        std::string variantProductsPath = variantEnvironment.resolve("EXECUTABLE_PATH") + variantEnvironment.resolve("EXECUTABLE_VARIANT_SUFFIX");
        std::string variantProductsOutput = productsDirectory + "/" + variantProductsPath;

        bool createUniversalBinary = targetEnvironment.architectures().size() > 1;
        std::vector<std::string> universalBinaryInputs;

        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment archEnvironment = variantEnvironment;
            archEnvironment.insertFront(ArchitectureLevel(arch));

            std::vector<pbxbuild::FileTypeResolver> files = ResolveBuildFiles(buildEnvironment, buildContext, archEnvironment, buildPhase->files());

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

    return invocations;
}

static std::vector<pbxbuild::ToolInvocation>
ShellScript(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxbuild::TargetEnvironment const &targetEnvironment, pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase)
{
    pbxspec::PBX::Tool::shared_ptr scriptTool = buildEnvironment.specManager()->tool("com.apple.commands.shell-script", targetEnvironment.specDomain());
    if (scriptTool == nullptr) {
        return std::vector<pbxbuild::ToolInvocation>();
    }

    std::string workingDirectory = targetEnvironment.workingDirectory();

    auto context = pbxbuild::ScriptInvocationContext::Create(scriptTool, buildPhase, targetEnvironment.environment(), workingDirectory);
    return { context.invocation() };
}

static std::vector<pbxproj::PBX::BuildPhase::shared_ptr>
SortBuildPhases(std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> phaseInvocations)
{
    std::unordered_map<std::string, pbxproj::PBX::BuildPhase::shared_ptr> outputToPhase;
    for (auto const &entry : phaseInvocations) {
        for (pbxbuild::ToolInvocation const &invocation : entry.second) {
            for (std::string const &output : invocation.outputs()) {
                outputToPhase.insert({ output, entry.first });
            }
        }
    }

    pbxbuild::BuildGraph<pbxproj::PBX::BuildPhase::shared_ptr> phaseGraph;
    for (auto const &entry : phaseInvocations) {
        phaseGraph.insert(entry.first, { });

        for (pbxbuild::ToolInvocation const &invocation : entry.second) {
            for (std::string const &input : invocation.inputs()) {
                auto it = outputToPhase.find(input);
                if (it != outputToPhase.end()) {
                    if (it->second != entry.first) {
                        phaseGraph.insert(entry.first, { it->second });
                    }
                }
            }
        }
    }

    return phaseGraph.ordered();
}

static std::vector<pbxbuild::ToolInvocation>
SortInvocations(std::vector<pbxbuild::ToolInvocation> invocations)
{
    std::unordered_map<std::string, pbxbuild::ToolInvocation const *> outputToInvocation;
    for (pbxbuild::ToolInvocation const &invocation : invocations) {
        for (std::string const &output : invocation.outputs()) {
            outputToInvocation.insert({ output, &invocation });
        }
    }

    pbxbuild::BuildGraph<pbxbuild::ToolInvocation const *> graph;
    for (pbxbuild::ToolInvocation const &invocation : invocations) {
        graph.insert(&invocation, { });

        for (std::string const &input : invocation.inputs()) {
            auto it = outputToInvocation.find(input);
            if (it != outputToInvocation.end()) {
                graph.insert(&invocation, { it->second });
            }
        }
    }

    std::vector<pbxbuild::ToolInvocation> result;
    for (pbxbuild::ToolInvocation const *invocation : graph.ordered()) {
        result.push_back(*invocation);
    }
    return result;
}

static void
BuildTarget(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironmentPtr = buildContext.targetEnvironment(buildEnvironment, target);
    if (targetEnvironmentPtr == nullptr) {
        fprintf(stderr, "error: couldn't create target environment\n");
        return;
    }
    pbxbuild::TargetEnvironment targetEnvironment = *targetEnvironmentPtr;

    // Filter build phases to ones appropriate for this target.
    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> buildPhases;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Check buildActionMask against buildContext.action.
        // TODO(grp): Check runOnlyForDeploymentPostprocessing.
        buildPhases.push_back(buildPhase);
    }

    std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> toolInvocations;

    std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> sourcesInvocations;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        if (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeSources) {
            auto BP = std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase);
            auto invocations = CompileFiles(buildEnvironment, buildContext, targetEnvironment, BP);
            sourcesInvocations.insert(invocations.begin(), invocations.end());

            std::vector<pbxbuild::ToolInvocation> allInvocations;
            for (auto const &pair : invocations) {
                allInvocations.insert(allInvocations.end(), pair.second.begin(), pair.second.end());
            }
            toolInvocations.insert({ buildPhase, allInvocations });
        }
    }

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeFrameworks: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::FrameworksBuildPhase> (buildPhase);
                auto invocations = LinkFiles(buildEnvironment, buildContext, targetEnvironment, BP, sourcesInvocations);
                toolInvocations.insert({ buildPhase, invocations });
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeShellScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ShellScriptBuildPhase> (buildPhase);
                auto invocations = ShellScript(buildEnvironment, buildContext, targetEnvironment, BP);
                toolInvocations.insert({ buildPhase, invocations });
                break;
            }
            default: break;
        }
    }


    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeShellScript:
            case pbxproj::PBX::BuildPhase::kTypeFrameworks:
            case pbxproj::PBX::BuildPhase::kTypeSources: {
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeHeaders: {
                // TODO: Copy Headers
                auto BP = std::static_pointer_cast <pbxproj::PBX::HeadersBuildPhase> (buildPhase);
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeResources: {
                // TODO: Copy Resources
                auto BP = std::static_pointer_cast <pbxproj::PBX::ResourcesBuildPhase> (buildPhase);
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeCopyFiles: {
                // TODO: Copy Files
                auto BP = std::static_pointer_cast <pbxproj::PBX::CopyFilesBuildPhase> (buildPhase);
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeAppleScript: {
                // TODO: Compile AppleScript
                auto BP = std::static_pointer_cast <pbxproj::PBX::AppleScriptBuildPhase> (buildPhase);
                break;
            }
        }
    }

    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> orderedPhases = SortBuildPhases(toolInvocations);

    printf("=== BUILD TARGET %s OF PROJECT %s WITH CONFIGURATION %s ===\n\n", target->name().c_str(), target->project()->name().c_str(), buildContext.configuration().c_str());
    printf("Check dependencies\n\n");

    printf("Write auxiliary files\n");
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : orderedPhases) {
        auto const entry = toolInvocations.find(buildPhase);
        for (pbxbuild::ToolInvocation const &invocation : entry->second) {
            for (std::string const &output : invocation.outputs()) {
                std::string directory = FSUtil::GetDirectoryName(output);
                if (!FSUtil::TestForDirectory(directory)) {
                    printf("/bin/mkdir -p %s\n", directory.c_str());
                    // TODO(grp): Create the directory.
                }
            }

            for (pbxbuild::ToolInvocation::AuxiliaryFile const &auxiliaryFile : invocation.auxiliaryFiles()) {
                if (!FSUtil::TestForRead(auxiliaryFile.path())) {
                    printf("write-file %s\n", auxiliaryFile.path().c_str());
                    // TODO(grp): Write the response file out.

                    if (auxiliaryFile.executable() && !FSUtil::TestForExecute(auxiliaryFile.path())) {
                        printf("chmod 0755 %s\n", auxiliaryFile.path().c_str());
                        // TODO(grp): Make the script executable.
                    }
                }
            }
        }
    }
    printf("\n");

    printf("Create product structure\n");
    // TODO(grp): Create the product structure.
    printf("\n");

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : orderedPhases) {
        auto const entry = toolInvocations.find(buildPhase);
        std::vector<pbxbuild::ToolInvocation> orderedInvocations = SortInvocations(entry->second);

        for (pbxbuild::ToolInvocation const &invocation : orderedInvocations) {
            printf("%s\n", invocation.logMessage().c_str());

            printf("\tcd %s\n", invocation.workingDirectory().c_str());
            // TODO(grp): Change into this directory.

            for (std::pair<std::string, std::string> const &entry : invocation.environment()) {
                printf("\texport %s=%s\n", entry.first.c_str(), entry.second.c_str());
            }
            printf("\t%s", invocation.executable().c_str());
            for (std::string const &arg : invocation.arguments()) {
                printf(" %s", arg.c_str());
            }
            printf("\n");
            // TODO(grp): Invoke command.

            printf("\tInputs:\n");
            for (std::string const &input : invocation.inputs()) {
                printf("\t\t%s\n", input.c_str());
            }
            printf("\tOutputs:\n");
            for (std::string const &output : invocation.outputs()) {
                printf("\t\t%s\n", output.c_str());
            }
            if (!invocation.auxiliaryFiles().empty()) {
                printf("\tAuxiliaries:\n");
                for (pbxbuild::ToolInvocation::AuxiliaryFile const &auxiliaryFile : invocation.auxiliaryFiles()) {
                    printf("\t\t%s\n", auxiliaryFile.path().c_str());
                }
            }

            printf("\n");
        }
    }
}

int
main(int argc, char **argv)
{
    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    if (argc < 5) {
        printf("Usage: %s workspace scheme config action\n", argv[0]);
        return -1;
    }

    xcworkspace::XC::Workspace::shared_ptr workspace = xcworkspace::XC::Workspace::Open(argv[1]);
    if (workspace == nullptr) {
        fprintf(stderr, "failed opening workspace\n");
        return -1;
    }

    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(workspace->projectFile(), workspace->name());
    if (group == nullptr) {
        fprintf(stderr, "failed opening scheme\n");
        return -1;
    }

    xcscheme::XC::Scheme::shared_ptr scheme = nullptr;
    for (xcscheme::XC::Scheme::shared_ptr const &available : group->schemes()) {
        printf("scheme: %s\n", available->name().c_str());
        if (available->name() == argv[2]) {
            scheme = available;
            break;
        }
    }
    if (scheme == nullptr) {
        fprintf(stderr, "couldn't find scheme\n");
        return -1;
    }

    pbxbuild::BuildContext context = pbxbuild::BuildContext::Workspace(
        workspace,
        scheme,
        argv[3],
        argv[4]
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    auto graph = resolver.resolveDependencies(context);
    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        BuildTarget(*buildEnvironment, context, target);
    }
}
