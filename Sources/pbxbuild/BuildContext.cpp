// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildContext.h>

using pbxbuild::BuildContext;

BuildContext::
BuildContext(std::shared_ptr<WorkspaceContext> const &workspaceContext, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration) :
    _workspaceContext  (workspaceContext),
    _scheme            (scheme),
    _targetEnvironments(std::make_shared<std::unordered_map<pbxproj::PBX::Target::shared_ptr, pbxbuild::TargetEnvironment>>()),
    _action            (action),
    _configuration     (configuration)
{
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

        pbxsetting::Setting::Parse("ARCHS", "armv7"),
    });
}

pbxsetting::Level BuildContext::
baseSettings(void) const
{
    std::string build = _workspaceContext->derivedDataName();

    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CONFIGURATION_BUILD_DIR", "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),
        pbxsetting::Setting::Parse("CONFIGURATION_TEMP_DIR", "$(PROJECT_TEMP_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),
        pbxsetting::Setting::Parse("SYMROOT", "$(DERIVED_DATA_DIR)/" + build + "/Build/Products"),
        pbxsetting::Setting::Parse("OBJROOT", "$(DERIVED_DATA_DIR)/" + build + "/Build/Intermediates"),
    });
}

BuildContext BuildContext::
Create(WorkspaceContext const &workspaceContext, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration)
{
    return BuildContext(std::make_shared<WorkspaceContext>(workspaceContext), scheme, action, configuration);
}

