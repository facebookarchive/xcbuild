// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/Manager.h>

using pbxspec::Manager;
using pbxspec::PBX::Specification;
using pbxspec::PBX::Architecture;
using pbxspec::PBX::BuildPhase;
using pbxspec::PBX::BuildSystem;
using pbxspec::PBX::Compiler;
using pbxspec::PBX::FileType;
using pbxspec::PBX::Linker;
using pbxspec::PBX::PackageType;
using pbxspec::PBX::ProductType;
using pbxspec::PBX::PropertyConditionFlavor;
using pbxspec::PBX::Tool;
using libutil::FSUtil;

Manager::Manager()
{
}

Manager::~Manager()
{
}

template <typename T>
typename T::shared_ptr
FindSpecification(
    std::map<char const *, Specification::vector> const &specifications,
    std::string const &identifier,
    char const *type = T::Type(),
    bool onlyDefault = false)
{
    if (type == nullptr) {
        return nullptr;
    }

    auto const &it = specifications.find(type);
    if (it == specifications.end()) {
        return nullptr;
    }

    auto const &vector = it->second;

    //
    // Do an inverse find so that we can find the overrides.
    //
    auto I = std::find_if(
            vector.rbegin(), vector.rend(),
            [&identifier,onlyDefault](Specification::shared_ptr const &spec) -> bool
            {
                return ((!onlyDefault || spec->isDefault()) &&
                        identifier == spec->identifier());
            });

    if (I == vector.rend()) {
        //
        // We couldn't find what the user wants, use any identifier found.
        //
        I = std::find_if(vector.rbegin(), vector.rend(),
                [&identifier](Specification::shared_ptr const &spec) -> bool
                {
                    return identifier == spec->identifier();
                });
        if (I == vector.rend())
            return nullptr;
    }

    return reinterpret_cast <typename T::shared_ptr const &> (*I);
}

Specification::shared_ptr Manager::
GetSpecification(char const *type, std::string const &identifier, bool onlyDefault) const
{
    return FindSpecification <Specification> (_specifications, identifier, type, onlyDefault);
}

Architecture::shared_ptr Manager::
GetArchitecture(std::string const &identifier) const
{
    return FindSpecification <Architecture> (_specifications, identifier);
}

BuildPhase::shared_ptr Manager::
GetBuildPhase(std::string const &identifier) const
{
    return FindSpecification <BuildPhase> (_specifications, identifier);
}

BuildSystem::shared_ptr Manager::
GetBuildSystem(std::string const &identifier) const
{
    return FindSpecification <BuildSystem> (_specifications, identifier);
}

Compiler::shared_ptr Manager::
GetCompiler(std::string const &identifier) const
{
    return FindSpecification <Compiler> (_specifications, identifier);
}

FileType::shared_ptr Manager::
GetFileType(std::string const &identifier) const
{
    return FindSpecification <FileType> (_specifications, identifier);
}

Linker::shared_ptr Manager::
GetLinker(std::string const &identifier) const
{
    return FindSpecification <Linker> (_specifications, identifier);
}

PackageType::shared_ptr Manager::
GetPackageType(std::string const &identifier) const
{
    return FindSpecification <PackageType> (_specifications, identifier);
}

ProductType::shared_ptr Manager::
GetProductType(std::string const &identifier) const
{
    return FindSpecification <ProductType> (_specifications, identifier);
}

PropertyConditionFlavor::shared_ptr Manager::
GetPropertyConditionFlavor(std::string const &identifier) const
{
    return FindSpecification <PropertyConditionFlavor> (_specifications, identifier);
}

Tool::shared_ptr Manager::
GetTool(std::string const &identifier) const
{
    return FindSpecification <Tool> (_specifications, identifier);
}

pbxsetting::Level Manager::
defaultSettings(void) const
{
    std::vector<PBX::PropertyOption::shared_ptr> options;

    auto const &bsit = _specifications.find(BuildSystem::Type());
    if (bsit != _specifications.end()) {
        for (std::shared_ptr<PBX::Specification> const &spec : bsit->second) {
            std::shared_ptr<PBX::BuildSystem> buildSystem = reinterpret_cast <BuildSystem::shared_ptr const &> (spec);
            options.insert(options.end(), buildSystem->properties().begin(), buildSystem->properties().end());
            options.insert(options.end(), buildSystem->options().begin(), buildSystem->options().end());
        }
    }

    auto const &toolit = _specifications.find(Tool::Type());
    if (toolit != _specifications.end()) {
        for (std::shared_ptr<PBX::Specification> const &spec : toolit->second) {
            std::shared_ptr<PBX::Tool> tool = reinterpret_cast <Tool::shared_ptr const &> (spec);
            options.insert(options.end(), tool->options().begin(), tool->options().end());
        }
    }

    auto const &compilerit = _specifications.find(Compiler::Type());
    if (compilerit != _specifications.end()) {
        for (std::shared_ptr<PBX::Specification> const &spec : compilerit->second) {
            std::shared_ptr<PBX::Compiler> compiler = reinterpret_cast <Compiler::shared_ptr const &> (spec);
            options.insert(options.end(), compiler->options().begin(), compiler->options().end());
        }
    }

    auto const &linkerit = _specifications.find(Linker::Type());
    if (linkerit != _specifications.end()) {
        for (std::shared_ptr<PBX::Specification> const &spec : linkerit->second) {
            std::shared_ptr<PBX::Linker> linker = reinterpret_cast <Linker::shared_ptr const &> (spec);
            options.insert(options.end(), linker->options().begin(), linker->options().end());
        }
    }

    std::vector<pbxsetting::Setting> settings;
    std::transform(options.begin(), options.end(), std::back_inserter(settings), [](PBX::PropertyOption::shared_ptr const &option) -> pbxsetting::Setting {
        if (option->defaultValue() == nullptr) {
            return pbxsetting::Setting::Parse(option->name(), "");
        } else if (plist::String const *stringValue = plist::CastTo <plist::String> (option->defaultValue())) {
            return pbxsetting::Setting::Parse(option->name(), stringValue->value());
        } else if (plist::Boolean const *booleanValue = plist::CastTo <plist::Boolean> (option->defaultValue())) {
            return pbxsetting::Setting::Parse(option->name(), booleanValue->value() ? "YES" : "NO");
        } else {
            // TODO(grp): Handle additional types?
            fprintf(stderr, "Warning: Unknown value type for setting %s.\n", option->name().c_str());
            return pbxsetting::Setting::Parse(option->name(), "");
        }
    });

    auto const &archit = _specifications.find(Architecture::Type());
    if (archit != _specifications.end()) {
        for (std::shared_ptr<PBX::Specification> const &spec : archit->second) {
            std::shared_ptr<PBX::Architecture> architecture = reinterpret_cast <Architecture::shared_ptr const &> (spec);
            if (!architecture->architectureSetting().empty()) {
                std::string value;
                for (std::string const &arch : architecture->realArchitectures()) {
                    if (&arch != &architecture->realArchitectures()[0]) {
                        value += " ";
                    }
                    value += arch;
                }

                settings.push_back(pbxsetting::Setting::Parse(architecture->architectureSetting(), value));
            }
        }
    }

    return pbxsetting::Level(settings);
}

void Manager::
AddSpecification(PBX::Specification::shared_ptr const &spec)
{
    if (!spec) {
        fprintf(stderr, "error: registering null specification\n");
        return;
    }

    if (spec->type() == nullptr) {
        fprintf(stderr, "error: registering a specification with null type\n");
        return;
    }

    if (auto ospec = GetSpecification(spec->type(), spec->identifier(), spec->isDefault())) {
        if (ospec->isDefault() && spec->isDefault()) {
            fprintf(stderr, "error: registering %s specification '%s' twice\n",
                    spec->type(), spec->identifier().c_str());
            return;
        }
    }

#if 0
    fprintf(stderr, "adding %s spec '%s'%s\n",
            spec->type(), spec->identifier().c_str(),
            spec->isDefault() ? "" : " [override]");
#endif
    _specifications[spec->type()].push_back(spec);
}

Manager::shared_ptr Manager::
Create(void)
{
    return std::make_shared <Manager> ();
}
