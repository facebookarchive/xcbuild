// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/Target.h>
#include <pbxproj/PBX/NativeTarget.h>
#include <pbxproj/PBX/BuildPhases.h>

using pbxproj::PBX::Target;
using pbxproj::PBX::NativeTarget;
using pbxproj::PBX::FileReference;
using pbxsetting::Level;
using pbxsetting::Setting;

Target::Target(std::string const &isa, Type type) :
    Object(isa),
    _type (type)
{
}

Level Target::
settings(void) const
{
    std::vector<Setting> settings = {
        Setting::Parse("TARGETNAME", _name),
        Setting::Parse("TARGET_NAME", _name),
        Setting::Parse("PRODUCT_NAME", _productName),
    };

    if (_type == kTypeNative) {
        NativeTarget const *nativeTarget = static_cast <NativeTarget const *> (this);
        if (FileReference::shared_ptr const &productReference = nativeTarget->productReference()) {
            Setting setting = Setting::Parse("FULL_PRODUCT_NAME", productReference->name());
            settings.push_back(setting);
        }
    }

    return Level(settings);
}

bool Target::
parse(Context &context, plist::Dictionary const *dict)
{
    _project = context.project;

    std::string BCLID;

    auto N   = dict->value <plist::String> ("name");
    auto PN  = dict->value <plist::String> ("productName");
    auto BCL = context.indirect <XC::ConfigurationList> (dict, "buildConfigurationList", &BCLID);
    auto BPs = dict->value <plist::Array> ("buildPhases");
    auto Ds  = dict->value <plist::Array> ("dependencies");

    if (N != nullptr) {
        _name = N->value();
    }

    if (PN != nullptr) {
        _productName = PN->value();
    }

    if (BCL != nullptr) {
        _buildConfigurationList =
          context.parseObject(context.configurationLists, BCLID, BCL);
        if (!_buildConfigurationList) {
            abort();
            return false;
        }
    }

    if (BPs != nullptr) {
        for (size_t n = 0; n < BPs->count(); n++) {
            auto ID = BPs->value <plist::String> (n);
            if (ID == nullptr)
                continue;

            if (auto BPd = context.get <HeadersBuildPhase> (ID)) {
                auto O = context.parseObject(context.headersBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else if (auto BPd = context.get <SourcesBuildPhase> (ID)) {
                auto O = context.parseObject(context.sourcesBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else if (auto BPd = context.get <ResourcesBuildPhase> (ID)) {
                auto O = context.parseObject(context.resourcesBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else if (auto BPd = context.get <FrameworksBuildPhase> (ID)) {
                auto O = context.parseObject(context.frameworksBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else if (auto BPd = context.get <CopyFilesBuildPhase> (ID)) {
                auto O = context.parseObject(context.copyFilesBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else if (auto BPd = context.get <ShellScriptBuildPhase> (ID)) {
                auto O = context.parseObject(context.shellScriptBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else if (auto BPd = context.get <AppleScriptBuildPhase> (ID)) {
                auto O = context.parseObject(context.appleScriptBuildPhases, ID->value(), BPd);
                if (!O) {
                    abort();
                    return false;
                }

                _buildPhases.push_back(O);
            } else {
                fprintf(stderr, "warning: target '%s' contains unsupported build phase reference to '%s'\n",
                        _name.c_str(), ID->value().c_str());
            }
        }
    }

    if (Ds != nullptr) {
        for (size_t n = 0; n < Ds->count(); n++) {
            std::string DID;
            auto D = context.get <TargetDependency> (Ds->value(n), &DID);
            if (D != nullptr) {
                auto TD = context.parseObject(context.targetDependencies, DID, D);
                if (!TD) {
                    abort();
                    return false;
                }

                _dependencies.push_back(TD);
            }
        }
    }

    return true;
}
