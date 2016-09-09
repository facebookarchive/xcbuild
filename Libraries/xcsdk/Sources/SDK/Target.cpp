/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/SDK/Target.h>
#include <xcsdk/SDK/Manager.h>
#include <pbxsetting/Type.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <plist/Array.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <plist/Format/Any.h>

using xcsdk::SDK::Target;
using libutil::Filesystem;
using libutil::FSUtil;

Target::
Target() :
    _customProperties (pbxsetting::Level({ })),
    _defaultProperties(pbxsetting::Level({ }))
{
}

Target::
~Target()
{
}

pbxsetting::Level Target::
settings(void) const
{
    std::vector<pbxsetting::Setting> settings = {
        pbxsetting::Setting::Create("SDK_NAME", _canonicalName),
        pbxsetting::Setting::Create("SDK_DIR", _path),
        pbxsetting::Setting::Create("SDK_PRODUCT_BUILD_VERSION", (_product ? _product->buildVersion() : "")),
    };

    std::vector<std::string> toolchainIdentifiers;
    for (Toolchain::shared_ptr const &toolchain : _toolchains) {
        toolchainIdentifiers.push_back(toolchain->identifier());
    }
    settings.push_back(pbxsetting::Setting::Create("TOOLCHAINS", pbxsetting::Type::FormatList(toolchainIdentifiers)));

    return pbxsetting::Level(settings);
}

std::vector<std::string> Target::
executablePaths() const
{
    std::vector<std::string> paths;
    return paths;
}

bool Target::
parse(plist::Dictionary const *dict)
{
    auto V    = dict->value <plist::String> ("Version");
    auto CN   = dict->value <plist::String> ("CanonicalName");
    auto DN   = dict->value <plist::String> ("DisplayName");
    auto MDN  = dict->value <plist::String> ("MinimalDisplayName");
    auto MDT  = dict->value <plist::String> ("MaximumDeploymentTarget");
    auto SBTV = dict->value <plist::Array> ("SupportedBuildToolsVersion");
    auto CP   = dict->value <plist::Dictionary> ("CustomProperties");
    auto DP   = dict->value <plist::Dictionary> ("DefaultProperties");
    auto IBS  = dict->value <plist::String> ("IsBaseSDK");
    auto TCV  = dict->value <plist::Array> ("Toolchains");

    if (V != nullptr) {
        _version = V->value();
    }

    if (CN != nullptr) {
        _canonicalName = CN->value();
    }

    if (DN != nullptr) {
        _displayName = DN->value();
    }

    if (MDN != nullptr) {
        _minimalDisplayName = MDN->value();
    }

    if (MDT != nullptr) {
        _maximumDeploymentTarget = MDT->value();
    }

    if (SBTV != nullptr) {
        for (size_t n = 0; n < SBTV->count(); n++) {
            auto SBT = SBTV->value <plist::String> (n);
            if (SBT != nullptr) {
                _supportedBuildToolsVersion.push_back(SBT->value());
            }
        }
    }

    if (CP != nullptr) {
        std::vector<pbxsetting::Setting> settings;
        for (size_t n = 0; n < CP->count(); n++) {
            auto CPK = CP->key(n);
            auto CPV = CP->value <plist::String> (CPK);

            if (CPV != nullptr) {
                pbxsetting::Setting setting = pbxsetting::Setting::Parse(CPK, CPV->value());
                settings.push_back(setting);
            }
        }
        _customProperties = pbxsetting::Level(settings);
    }

    if (DP != nullptr) {
        std::vector<pbxsetting::Setting> settings;
        for (size_t n = 0; n < DP->count(); n++) {
            auto DPK = DP->key(n);
            auto DPV = DP->value <plist::String> (DPK);

            if (DPV != nullptr) {
                pbxsetting::Setting setting = pbxsetting::Setting::Parse(DPK, DPV->value());
                settings.push_back(setting);
            }
        }
        _defaultProperties = pbxsetting::Level(settings);
    }

    if (IBS != nullptr) {
        _isBaseSDK = pbxsetting::Type::ParseBoolean(IBS->value());
    }

    if (TCV != nullptr) {
        if (std::shared_ptr<Manager> manager = _manager.lock()) {
            for (size_t n = 0; n < TCV->count(); n++) {
                auto TCI = TCV->value <plist::String> (n);
                if (TCI != nullptr) {
                    if (auto toolchain = manager->findToolchain(TCI->value())) {
                        _toolchains.push_back(toolchain);
                    } else if (auto defaultToolchain = manager->findToolchain(Toolchain::DefaultIdentifier())) {
                        _toolchains.push_back(defaultToolchain);
                    }
                }
            }
        }
    } else {
        if (std::shared_ptr<Manager> manager = _manager.lock()) {
            if (auto defaultToolchain = manager->findToolchain(Toolchain::DefaultIdentifier())) {
                _toolchains.push_back(defaultToolchain);
            }
        }
    }

    return true;
}

Target::shared_ptr Target::
Open(Filesystem const *filesystem, std::shared_ptr<Manager> manager, std::shared_ptr<Platform> platform, std::string const &path)
{
    if (path.empty()) {
        return nullptr;
    }

    std::string settingsFileName = path + "/SDKSettings.plist";
    if (!filesystem->isReadable(settingsFileName.c_str())) {
        settingsFileName = path + "/Info.plist";
        if (!filesystem->isReadable(settingsFileName.c_str())) {
            return nullptr;
        }
    }

    std::string realPath = filesystem->resolvePath(settingsFileName);
    if (realPath.empty()) {
        return nullptr;
    }

    std::vector<uint8_t> contents;
    if (!filesystem->read(&contents, settingsFileName)) {
        return nullptr;
    }

    //
    // Parse property list
    //
    auto result = plist::Format::Any::Deserialize(contents);
    if (result.first == nullptr) {
        return nullptr;
    }

    plist::Dictionary *plist = plist::CastTo<plist::Dictionary>(result.first.get());
    if (plist == nullptr) {
        return nullptr;
    }

    //
    // Parse the SDK dictionary and create the object.
    //
    auto target = std::make_shared<Target>();
    target->_manager  = manager;
    target->_platform = platform;

    if (!target->parse(plist)) {
        return nullptr;
    }

    //
    // Save some useful info
    //
    target->_path       = FSUtil::GetDirectoryName(realPath);
    target->_bundleName = FSUtil::GetBaseName(realPath);

    //
    // Parse product information
    //
    target->_product = Product::Open(filesystem, target->_path);

    return target;
}
