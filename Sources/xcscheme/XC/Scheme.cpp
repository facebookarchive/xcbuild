// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/Scheme.h>
#include <xcscheme/XC/Actions.h>

using xcscheme::XC::Scheme;
using libutil::FSUtil;

Scheme::Scheme(std::string const &name, std::string const &owner) :
    _name              (name),
    _owner             (owner),
    _lastUpgradeVersion(0)
{
}

bool Scheme::
parse(plist::Dictionary const *dict)
{
    auto S = dict->value <plist::Dictionary> ("Scheme");
    if (S == nullptr)
        return false;

    auto LUV = S->value <plist::Integer> ("LastUpgradeVersion");
    auto V   = S->value <plist::String> ("version");
    auto BA  = S->value <plist::Dictionary> ("BuildAction");
    auto TA  = S->value <plist::Dictionary> ("TestAction");
    auto LA  = S->value <plist::Dictionary> ("LaunchAction");
    auto PA  = S->value <plist::Dictionary> ("ProfileAction");
    auto AnA = S->value <plist::Dictionary> ("AnalyzeAction");
    auto ArA = S->value <plist::Dictionary> ("ArchiveAction");

    if (LUV != nullptr) {
        _lastUpgradeVersion = LUV->value();
    }

    if (V != nullptr) {
        _version = V->value();
    }

    if (BA != nullptr) {
        _buildAction = std::make_shared <BuildAction> ();
        if (!_buildAction->parse(BA))
            return false;
    }

    if (TA != nullptr) {
        _testAction = std::make_shared <TestAction> ();
        if (!_testAction->parse(TA))
            return false;
    }

    if (LA != nullptr) {
        _launchAction = std::make_shared <LaunchAction> ();
        if (!_launchAction->parse(LA))
            return false;
    }

    if (PA != nullptr) {
        _profileAction = std::make_shared <ProfileAction> ();
        if (!_profileAction->parse(PA))
            return false;
    }

    if (AnA != nullptr) {
        _analyzeAction = std::make_shared <AnalyzeAction> ();
        if (!_analyzeAction->parse(AnA))
            return false;
    }

    if (ArA != nullptr) {
        _archiveAction = std::make_shared <ArchiveAction> ();
        if (!_archiveAction->parse(ArA))
            return false;
    }

    return true;
}

Scheme::shared_ptr Scheme::
Open(std::string const &name, std::string const &owner, std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    if (!FSUtil::TestForRead(path.c_str()))
        return nullptr;

    std::string realPath = FSUtil::ResolvePath(path);
    if (realPath.empty())
        return nullptr;

    //
    // Parse simple XML
    //
    plist::Dictionary *plist = plist::Dictionary::ParseSimpleXML(path);
    if (plist == nullptr)
        return nullptr;

#if 0
    plist->dump(stdout);
#endif

    //
    // Parse the scheme dictionary and create the scheme object.
    //
    auto scheme = std::make_shared <Scheme> (name, owner);
    if (scheme->parse(plist)) {
        scheme->_path = realPath;
    } else {
        scheme = nullptr;
    }
                                                        
    //
    // Release the property list.
    //
    plist->release();

    return scheme;
}
