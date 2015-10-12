// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcdriver/BuildAction.h>
#include <xcdriver/Action.h>
#include <xcdriver/Options.h>

using xcdriver::BuildAction;
using xcdriver::Options;
using libutil::FSUtil;

BuildAction::
BuildAction()
{
}

BuildAction::
~BuildAction()
{
}

static bool
VerifyBuildActions(std::vector<std::string> const &actions)
{
    for (std::string const &action : actions) {
        if (action != "build" &&
            action != "analyze" &&
            action != "archive" &&
            action != "test" &&
            action != "installsrc" &&
            action != "install" &&
            action != "clean") {
            fprintf(stderr, "error: unknown build action '%s'\n", action.c_str());
            return false;
        }
    }

    return true;
}

static std::vector<pbxsetting::Level>
OverrideLevels(Options const &options, pbxsetting::Environment const &environment)
{
    std::vector<pbxsetting::Level> levels;

    std::vector<pbxsetting::Setting> settings;
    if (!options.sdk().empty()) {
        settings.push_back(pbxsetting::Setting::Parse("SDKROOT", options.sdk()));
    }
    if (!options.arch().empty()) {
        settings.push_back(pbxsetting::Setting::Parse("ARCHS", options.arch()));
    }
    levels.push_back(pbxsetting::Level(settings));

    levels.push_back(options.settings());

    if (!options.xcconfig().empty()) {
        pbxsetting::XC::Config::shared_ptr config = pbxsetting::XC::Config::Open(options.xcconfig(), environment);
        if (config == nullptr) {
            fprintf(stderr, "warning: unable to open xcconfig '%s'\n", options.xcconfig().c_str());
        } else {
            levels.push_back(config->level());
        }
    }

    if (getenv("XCODE_XCCONFIG_FILE")) {
        std::string path = getenv("XCODE_XCCONFIG_FILE");

        pbxsetting::XC::Config::shared_ptr config = pbxsetting::XC::Config::Open(path, environment);
        if (config == nullptr) {
            fprintf(stderr, "warning: unable to open xcconfig from environment '%s'\n", path.c_str());
        } else {
            levels.push_back(config->level());
        }
    }

    return levels;
}

static bool
VerifySupportedOptions(Options const &options)
{
    if (!options.toolchain().empty()) {
        fprintf(stderr, "warning: toolchain option not implemented\n");
    }

    if (!options.destination().empty() || !options.destinationTimeout().empty()) {
        fprintf(stderr, "warning: destination option not implemented\n");
    }

    if (options.parallelizeTargets() || options.jobs() > 0) {
        fprintf(stderr, "warning: job control option not implemented\n");
    }

    if (options.hideShellScriptEnvironment() || options.showBuildSettings()) {
        fprintf(stderr, "warning: output control option not implemented\n");
    }

    if (options.enableAddressSanitizer() || options.enableCodeCoverage()) {
        fprintf(stderr, "warning: build mode option not implemented\n");
    }

    if (!options.derivedDataPath().empty()) {
        fprintf(stderr, "warning: custom derived data path not implemented\n");
    }

    if (!options.resultBundlePath().empty()) {
        fprintf(stderr, "warning: result bundle path not implemented\n");
    }

    return true;
}

int BuildAction::
Run(Options const &options)
{
    // TODO(grp): Implement these options.
    if (!VerifySupportedOptions(options)) {
        return -1;
    }

    if (!VerifyBuildActions(options.actions())) {
        return -1;
    }

    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    std::unique_ptr<pbxbuild::WorkspaceContext> workspaceContext = Action::CreateWorkspace(options);
    if (workspaceContext == nullptr) {
        return -1;
    }

    xcscheme::XC::Scheme::shared_ptr scheme = nullptr;
    if (!options.scheme().empty()) {
        scheme = workspaceContext->scheme(options.scheme());
    }

    std::vector<pbxsetting::Level> overrideLevels = OverrideLevels(options, buildEnvironment->baseEnvironment());

    std::vector<std::string> actions = (!options.actions().empty() ? options.actions() : std::vector<std::string>({ "build" }));
    std::string action = actions.front(); // TODO(grp): Support multiple actions and skipUnavailableOptions.

    if (action != "build") {
        fprintf(stderr, "error: action '%s' is not implemented\n", action.c_str());
        return -1;
    }

    std::string configuration = options.configuration();
    bool defaultConfiguration = false;
    if (configuration.empty()) {
        if (scheme != nullptr) {
            configuration = scheme->buildAction()->buildConfiguration();
            if (configuration.empty()) {
                configuration = "Debug";
            }
        } else if (workspaceContext->project() != nullptr) {
            defaultConfiguration = true;
            configuration = workspaceContext->project()->buildConfigurationList()->defaultConfigurationName();
        }

        if (configuration.empty()) {
            fprintf(stderr, "error: unable to determine build configuration\n");
            return -1;
        }
    }

    pbxbuild::BuildContext buildContext = pbxbuild::BuildContext::Create(
        *workspaceContext,
        scheme,
        action,
        configuration,
        defaultConfiguration,
        overrideLevels
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    pbxbuild::BuildGraph<pbxproj::PBX::Target::shared_ptr> graph;
    if (scheme != nullptr) {
        graph = resolver.resolveSchemeDependencies(buildContext);
    } else if (workspaceContext->project() != nullptr) {
        graph = resolver.resolveLegacyDependencies(buildContext, options.allTargets(), options.target());
    } else {
        fprintf(stderr, "error: scheme is required for workspace\n");
        return -1;
    }

    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    bool color = isatty(fileno(stdout));
    std::shared_ptr<pbxbuild::Build::DefaultFormatter> formatter = pbxbuild::Build::DefaultFormatter::Create(color);
    pbxbuild::Build::Formatter::Print(formatter->begin(buildContext));

    std::unique_ptr<pbxbuild::Build::SimpleExecutor> executor = pbxbuild::Build::SimpleExecutor::Create(
        *buildEnvironment,
        buildContext,
        std::static_pointer_cast<pbxbuild::Build::Formatter>(formatter),
        options.dryRun()
    );

    bool succeeded = true;
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        pbxbuild::Build::Formatter::Print(formatter->beginTarget(buildContext, target));
        std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = buildContext.targetEnvironment(*buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment\n");
            continue;
        }

        pbxbuild::Build::Formatter::Print(formatter->checkDependencies(target));
        pbxbuild::Phase::PhaseContext phaseContext = pbxbuild::Phase::PhaseContext(*buildEnvironment, buildContext, target, *targetEnvironment);
        pbxbuild::Phase::PhaseInvocations phaseInvocations = pbxbuild::Phase::PhaseInvocations::Create(phaseContext, target);

        if (!executor->buildTarget(target, *targetEnvironment, phaseInvocations.orderedPhases(), phaseInvocations.invocations())) {
            return 1;
        }
    }

    pbxbuild::Build::Formatter::Print(formatter->success(buildContext));
    return 0;
}
