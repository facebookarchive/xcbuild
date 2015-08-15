// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/SDK/Target.h>
#include <pbxsdk/SDK/Manager.h>

using pbxsdk::SDK::Target;
using libutil::FSUtil;

Target::Target() :
    _customProperties (nullptr),
    _defaultProperties(nullptr)
{
}

Target::~Target()
{
    if (_defaultProperties != nullptr) {
        _defaultProperties->release();
    }

    if (_customProperties != nullptr) {
        _customProperties->release();
    }
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
    auto CP   = dict->value <plist::String> ("CustomProperties");
    auto DP   = dict->value <plist::String> ("DefaultProperties");
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
        _customProperties = plist::CastTo <plist::Dictionary> (CP->copy());
    }

    if (DP != nullptr) {
        _defaultProperties = plist::CastTo <plist::Dictionary> (DP->copy());
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
                        TCII = TCS.find("com.apple.dt.toolchain.XcodeDefault");
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
