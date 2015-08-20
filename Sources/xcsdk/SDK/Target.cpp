// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcsdk/SDK/Target.h>
#include <xcsdk/SDK/Manager.h>

using xcsdk::SDK::Target;
using pbxsetting::Level;
using pbxsetting::Setting;
using libutil::FSUtil;

Target::Target() :
    _customProperties (Level({ })),
    _defaultProperties(Level({ }))
{
}

Target::~Target()
{
}

Level Target::
settings(void) const
{
    std::vector<Setting> settings = {
        Setting::Parse("SDK_NAME", _canonicalName),
        Setting::Parse("SDK_DIR", _path),
        Setting::Parse("SDK_PRODUCT_BUILD_VERSION", _product ? _product->buildVersion() : ""),
    };

    std::string toolchains;
    for (Toolchain::shared_ptr const &toolchain : _toolchains) {
        if (&toolchain != &_toolchains[0]) {
            toolchains += " ";
        }
        toolchains += toolchain->identifier();
    }
    settings.push_back(Setting::Parse("TOOLCHAINS", toolchains));

    return Level(settings);
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
    auto IBS  = dict->value <plist::Boolean> ("IsBaseSDK");
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
        std::vector<Setting> settings;
        for (size_t n = 0; n < CP->count(); n++) {
            auto CPK = CP->key(n);
            auto CPV = CP->value <plist::String> (CPK);

            if (CPV != nullptr) {
                Setting setting = Setting::Parse(CPK, CPV->value());
                settings.push_back(setting);
            }
        }
        _customProperties = Level(settings);
    }

    if (DP != nullptr) {
        std::vector<Setting> settings;
        for (size_t n = 0; n < DP->count(); n++) {
            auto DPK = DP->key(n);
            auto DPV = DP->value <plist::String> (DPK);

            if (DPV != nullptr) {
                Setting setting = Setting::Parse(DPK, DPV->value());
                settings.push_back(setting);
            }
        }
        _defaultProperties = Level(settings);
    }

    if (IBS != nullptr) {
        _isBaseSDK = IBS->value();
    }

    if (TCV != nullptr) {
        if (std::shared_ptr<Manager> manager = _manager.lock()) {
            for (size_t n = 0; n < TCV->count(); n++) {
                auto TCI = TCV->value <plist::String> (n);
                if (TCI != nullptr) {
                    auto TCS = manager->toolchains();
                    auto TCII = TCS.find(TCI->value());
                    if (TCII == TCS.end()) {
                        TCII = TCS.find(Toolchain::DefaultIdentifier());
                    }

                    if (TCII != TCS.end()) {
                        _toolchains.push_back(TCII->second);
                    }
                }
            }
        }
    }

    return true;
}

Target::shared_ptr Target::
Open(std::shared_ptr<Manager> manager, std::shared_ptr<Platform> platform, std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    std::string settingsFileName = path + "/SDKSettings.plist";
    if (!FSUtil::TestForRead(settingsFileName.c_str())) {
        settingsFileName = path + "/Info.plist";
        if (!FSUtil::TestForRead(settingsFileName.c_str())) {
            return nullptr;
        }
    }

    std::string realPath = FSUtil::ResolvePath(settingsFileName);
    if (realPath.empty())
        return nullptr;

    //
    // Parse property list
    //
    plist::Dictionary *plist = plist::Dictionary::Parse(settingsFileName);
    if (plist == nullptr)
        return nullptr;

#if 0
    plist->dump(stdout);
#endif

    //
    // Parse the SDK dictionary and create the object.
    //
    auto target = std::make_shared <Target> ();
    target->_manager    = manager;
    target->_platform   = platform;

    if (target->parse(plist)) {
        //
        // Save some useful info
        //
        target->_path       = FSUtil::GetDirectoryName(realPath);

        size_t slash = target->_path.rfind('/');

        target->_bundleName = target->_path.substr(slash + 1);
    } else {
        target = nullptr;
    }

    //
    // Release the property list.
    //
    plist->release();

    if (target) {
        //
        // Parse product information
        //
        target->_product = Product::Open(target->_path);
    }

    return target;
}
