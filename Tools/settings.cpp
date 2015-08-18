// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>

extern char **environ;

static std::map<std::string, std::string>
environmentVariables(void)
{
    std::map<std::string, std::string> environment;

    for (char **current = environ; *current; current++) {
        std::string variable = *current;
        std::string::size_type offset = variable.find('=');

        std::string name = variable.substr(0, offset);
        std::string value = variable.substr(offset + 1);
        environment.insert(std::make_pair(name, value));
    }

    return environment;
}

static pbxsetting::Level
environmentLevel(void)
{
    std::vector<pbxsetting::Setting> environmentSettings;
    for (std::pair<std::string, std::string> const &variable : environmentVariables()) {
        // TODO(grp): Is this right? Should this be filtered at another level?
        if (variable.first.front() != '_') {
            pbxsetting::Setting setting = pbxsetting::Setting::Parse(variable.first, variable.second);
            environmentSettings.push_back(setting);
        }
    }
    return pbxsetting::Level(environmentSettings);
}

int
main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s developer project specs\n", argv[0]);
        return -1;
    }

    std::shared_ptr<xcsdk::SDK::Manager> xcsdk_manager = xcsdk::SDK::Manager::Open(argv[1]);
    if (!xcsdk_manager) {
        fprintf(stderr, "no developer dir found at %s\n", argv[1]);
        return -1;
    }

    auto project = pbxproj::PBX::Project::Open(argv[2]);
    if (!project) {
        fprintf(stderr, "error opening project at %s (%s)\n", argv[2], strerror(errno));
        return -1;
    }

    auto spec_manager = pbxspec::Manager::Open(argv[3]);
    if (!spec_manager) {
        fprintf(stderr, "error opening specifications at %s (%s)\n", argv[3], strerror(errno));
        return -1;
    }

    std::vector<pbxsetting::Level> levels;

    printf("Project: %s\n", project->name().c_str());

    auto projectConfigurationList = project->buildConfigurationList();
    std::string projectConfigurationName = projectConfigurationList->defaultConfigurationName();
    auto projectConfiguration = *std::find_if(projectConfigurationList->begin(), projectConfigurationList->end(), [&](pbxproj::XC::BuildConfiguration::shared_ptr configuration) -> bool {
        return configuration->name() == projectConfigurationName;
    });
    printf("Project Configuration: %s\n", projectConfiguration->name().c_str());

    auto target = project->targets().front();
    printf("Target: %s\n", target->name().c_str());

    auto targetConfigurationList = target->buildConfigurationList();
    std::string targetConfigurationName = targetConfigurationList->defaultConfigurationName();
    auto targetConfiguration = *std::find_if(targetConfigurationList->begin(), targetConfigurationList->end(), [&](pbxproj::XC::BuildConfiguration::shared_ptr configuration) -> bool {
        return configuration->name() == targetConfigurationName;
    });
    printf("Target Configuration: %s\n", targetConfiguration->name().c_str());

    auto platform = *std::find_if(xcsdk_manager->platforms().begin(), xcsdk_manager->platforms().end(), [](std::shared_ptr<xcsdk::SDK::Platform> platform) -> bool {
        return platform->name() == "iphoneos";
    });
    printf("Platform: %s\n", platform->name().c_str());

    auto sdk = platform->targets().front();
    printf("SDK: %s\n", sdk->displayName().c_str());

    pbxsetting::Level specDefaultSettings = spec_manager->defaultSettings();

    // TODO(grp): targetConfiguration->baseConfigurationReference()
    levels.push_back(targetConfiguration->buildSettings());
    // TODO(grp): projectConfiguration->baseConfigurationReference()
    levels.push_back(projectConfiguration->buildSettings());

    levels.push_back(platform->overrideProperties());
    levels.push_back(specDefaultSettings);
    levels.push_back(sdk->customProperties());
    levels.push_back(sdk->defaultProperties());
    levels.push_back(platform->defaultProperties());
    // TODO(grp): system defaults?
    levels.push_back(environmentLevel());

    pbxsetting::Environment environment = pbxsetting::Environment(levels, levels);

    pbxsetting::Condition condition = pbxsetting::Condition({
        { "sdk", sdk->canonicalName() },
        { "arch", "arm64" }, // TODO(grp): Use a real architcture.
        { "variant", "default" },
    });

    std::unordered_map<std::string, std::string> values = environment.computeValues(condition);
    std::map<std::string, std::string> orderedValues = std::map<std::string, std::string>(values.begin(), values.end());

    printf("\n\nBuild Settings:\n\n");
    for (auto const &value : orderedValues) {
        auto defaultValue = specDefaultSettings.get(value.first, condition);
        if (!defaultValue.first || value.second != defaultValue.second.raw()) {
            printf("%s = %s\n", value.first.c_str(), value.second.c_str());
        }
    }

    return 0;
}
