// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildContext.h>
#include <libutil/md5.h>

using pbxbuild::BuildContext;

BuildContext::
BuildContext(pbxproj::PBX::Project::shared_ptr const &project, xcworkspace::XC::Workspace::shared_ptr const &workspace, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration) :
    _project(project),
    _workspace(workspace),
    _scheme(scheme),
    _projects(std::make_shared<std::map<std::string, pbxproj::PBX::Project::shared_ptr>>()),
    _targetEnvironments(std::make_shared<std::map<pbxproj::PBX::Target::shared_ptr, pbxbuild::TargetEnvironment>>()),
    _action(action),
    _configuration(configuration)
{
}

pbxproj::PBX::Project::shared_ptr BuildContext::
project(std::string const &projectPath) const
{
    auto PI = _projects->find(projectPath);
    if (PI != _projects->end()) {
        return PI->second;
    } else {
        pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(projectPath);
        if (project != nullptr) {
            _projects->insert(std::make_pair(projectPath, project));
        } else {
            fprintf(stderr, "warning: unable to load project at %s\n", projectPath.c_str());
        }
        return project;
    }
}

std::unique_ptr<pbxbuild::TargetEnvironment> BuildContext::
targetEnvironment(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target) const
{
    auto TEI = _targetEnvironments->find(target);
    if (TEI != _targetEnvironments->end()) {
        return std::make_unique<pbxbuild::TargetEnvironment>(TEI->second);
    } else {
        std::unique_ptr<TargetEnvironment> targetEnvironment = TargetEnvironment::Create(buildEnvironment, target, this);
        if (targetEnvironment != nullptr) {
            _targetEnvironments->insert(std::make_pair(target, *targetEnvironment));
        }
        return targetEnvironment;
    }
}

pbxproj::PBX::Target::shared_ptr BuildContext::
resolveTargetIdentifier(pbxproj::PBX::Project::shared_ptr const &project, std::string const &identifier) const
{
    if (project == nullptr) {
        return nullptr;
    }

    pbxproj::PBX::Target::shared_ptr foundTarget = nullptr;
    for (pbxproj::PBX::Target::shared_ptr const &target : project->targets()) {
        if (target->blueprintIdentifier() == identifier) {
            return target;
        }
    }

    return nullptr;
}

std::unique_ptr<std::pair<pbxproj::PBX::Target::shared_ptr, pbxproj::PBX::FileReference::shared_ptr>> BuildContext::
resolveProductIdentifier(pbxproj::PBX::Project::shared_ptr const &project, std::string const &identifier) const
{
    if (project == nullptr) {
        return nullptr;
    }

    pbxproj::PBX::Target::shared_ptr foundTarget = nullptr;
    for (pbxproj::PBX::Target::shared_ptr const &target : project->targets()) {
        if (target->type() == pbxproj::PBX::Target::kTypeNative) {
            pbxproj::PBX::NativeTarget::shared_ptr nativeTarget = std::static_pointer_cast<pbxproj::PBX::NativeTarget>(target);
            if (nativeTarget->productReference() != nullptr && nativeTarget->productReference()->blueprintIdentifier() == identifier) {
                auto pair = std::make_pair(target, nativeTarget->productReference());
                return std::make_unique<std::pair<pbxproj::PBX::Target::shared_ptr, pbxproj::PBX::FileReference::shared_ptr>>(pair);
            }
        }
    }

    return nullptr;
}

pbxsetting::Level BuildContext::
actionSettings(void) const
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("ACTION", _action),
        pbxsetting::Setting::Parse("BUILD_COMPONENTS", "headers build"), // TODO(grp): Should depend on action?

        pbxsetting::Setting::Parse("CONFIGURATION", _configuration),
    });
}

static uint64_t
hton64(uint64_t v)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    v = ((v & 0x00000000000000FFULL) << 56) |
        ((v & 0x000000000000FF00ULL) << 40) |
        ((v & 0x0000000000FF0000ULL) << 24) |
        ((v & 0x00000000FF000000ULL) <<  8) |
        ((v & 0x000000FF00000000ULL) >>  8) |
        ((v & 0x0000FF0000000000ULL) >> 24) |
        ((v & 0x00FF000000000000ULL) >> 40) |
        ((v & 0xFF00000000000000ULL) >> 56);
#endif
    return v;
}

static std::string
DerviedDataHash(std::string const &path)
{
    md5_state_t state;
    md5_init(&state);
    md5_append(&state, reinterpret_cast<const md5_byte_t *>(path.data()), path.size());

    uint8_t digest[16];
    md5_finish(&state, reinterpret_cast<md5_byte_t *>(&digest));

    char hash_path[28];
    int counter;

    uint64_t first_value = hton64(*reinterpret_cast<uint64_t *>(&digest[0]));
    counter = 13;
    while (counter >= 0) {
        hash_path[counter] = 'a' + (first_value % 26);
        first_value /= 26;
        counter--;
    }

    uint64_t second_value = hton64(*reinterpret_cast<uint64_t *>(&digest[8]));
    counter = 27;
    while (counter > 13) {
        hash_path[counter] = 'a' + (second_value % 26);
        second_value /= 26;
        counter--;
    }

    return std::string(hash_path, 28);
}

pbxsetting::Level BuildContext::
baseSettings(void) const
{
    std::string build;
    if (_workspace != nullptr) {
        build = _workspace->name() + "-" + DerviedDataHash(_workspace->projectFile());
    } else if (_project != nullptr) {
        build = _project->name() + "-" + DerviedDataHash(_project->projectFile());
    } else {
        fprintf(stderr, "error: build context without workspace or project\n");
    }

    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CONFIGURATION_BUILD_DIR", "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),
        pbxsetting::Setting::Parse("CONFIGURATION_TEMP_DIR", "$(PROJECT_TEMP_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),
        pbxsetting::Setting::Parse("SYMROOT", "$(DERIVED_DATA_DIR)/" + build + "/Build/Products"),
        pbxsetting::Setting::Parse("OBJROOT", "$(DERIVED_DATA_DIR)/" + build + "/Build/Intermediates"),
    });
}

BuildContext BuildContext::
Workspace(xcworkspace::XC::Workspace::shared_ptr const &workspace, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration)
{
    assert(workspace != nullptr);
    return BuildContext(nullptr, workspace, scheme, action, configuration);
}

BuildContext BuildContext::
Project(pbxproj::PBX::Project::shared_ptr const &project, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration)
{
    BuildContext buildContext = BuildContext(project, nullptr, scheme, action, configuration);
    buildContext._projects->insert(std::make_pair(project->projectFile(), project));
    return buildContext;
}
