// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/SDK/Platform.h>

using pbxsdk::SDK::Platform;
using libutil::FSUtil;

Platform::Platform() :
    _defaultDebuggerSettings(nullptr),
    _defaultProperties      (nullptr)
{
}

Platform::~Platform()
{
    if (_defaultProperties != nullptr) {
        _defaultProperties->release();
    }
    if (_defaultDebuggerSettings != nullptr) {
        _defaultDebuggerSettings->release();
    }
}

bool Platform::
parse(plist::Dictionary const *dict)
{
    auto I   = dict->value <plist::String> ("Identifier");
    auto N   = dict->value <plist::String> ("Name");
    auto D   = dict->value <plist::String> ("Description");
    auto T   = dict->value <plist::String> ("Type");
    auto V   = dict->value <plist::String> ("Version");
    auto FI  = dict->value <plist::String> ("FamilyIdentifier");
    auto FN  = dict->value <plist::String> ("FamilyName");
    auto Ic  = dict->value <plist::String> ("Icon");
    auto DDS = dict->value <plist::String> ("DefaultDebuggerSettings");
    auto DP  = dict->value <plist::String> ("DefaultProperties");

    if (I != nullptr) {
        _identifier = I->value();
    }

    if (N != nullptr) {
        _name = N->value();
    }

    if (D != nullptr) {
        _description = D->value();
    }

    if (T != nullptr) {
        _type = T->value();
    }

    if (V != nullptr) {
        _version = V->value();
    }

    if (FI != nullptr) {
        _familyIdentifier = FI->value();
    }

    if (FN != nullptr) {
        _familyName = FN->value();
    }

    if (Ic != nullptr) {
        _icon = Ic->value();
    }

    if (DDS != nullptr) {
        _defaultDebuggerSettings = plist::CastTo <plist::Dictionary> (DDS->copy());
    }

    if (DP != nullptr) {
        _defaultProperties = plist::CastTo <plist::Dictionary> (DP->copy());
    }

    return true;
}

Platform::shared_ptr Platform::
Open(std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    std::string settingsFileName = path + "/Info.plist";
    if (!FSUtil::TestForRead(settingsFileName.c_str()))
        return nullptr;

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
    // Parse the SDK platform dictionary and create the object.
    //
    auto platform = std::make_shared <Platform> ();

    if (platform->parse(plist)) {
        //
        // Save some useful info
        //
        platform->_path = FSUtil::GetDirectoryName(realPath);
    } else {
        platform = nullptr;
    }

    //
    // Release the property list.
    //
    plist->release();

    if (platform) {
        //
        // Lookup all the SDKs inside the platform
        //
        std::string sdksPath = platform->_path + "/Developer/SDKs";
        FSUtil::EnumerateDirectory(sdksPath, "*.sdk",
                [&](std::string const &filename) -> bool
                {
                    if (auto target = Target::Open(sdksPath + "/" + filename)) {
                        target->_platform = platform.get();
                        platform->_targets.push_back(target);
                    }
                    return true;
                });

        std::sort(platform->_targets.begin(), platform->_targets.end(),
                [](Target::shared_ptr const &a, Target::shared_ptr const &b) -> bool
                {
                    return (a->canonicalName() < b->canonicalName());
                });
    }

    return platform;
}
