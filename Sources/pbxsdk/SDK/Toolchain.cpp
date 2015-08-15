// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/SDK/Toolchain.h>

using pbxsdk::SDK::Toolchain;
using libutil::FSUtil;

Toolchain::Toolchain()
{
}

Toolchain::~Toolchain()
{
}

bool Toolchain::
parse(plist::Dictionary const *dict)
{
    auto I = dict->value <plist::String> ("Identifier");

    if (I != nullptr) {
        _identifier = I->value();
    }

    return true;
}

Toolchain::shared_ptr Toolchain::
Open(std::shared_ptr<Manager> manager, std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    std::string settingsFileName = path + "/ToolchainInfo.plist";
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
    // Parse the toolchain dictionary and create the object.
    //
    auto toolchain = std::make_shared <Toolchain> ();
    toolchain->_manager = manager;

    if (toolchain->parse(plist)) {
        //
        // Save some useful info
        //
        toolchain->_path = FSUtil::GetDirectoryName(realPath);
    } else {
        toolchain = nullptr;
    }

    //
    // Release the property list.
    //
    plist->release();

    return toolchain;
}
