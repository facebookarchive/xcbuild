/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/SDK/Toolchain.h>

using xcsdk::SDK::Toolchain;
using libutil::FSUtil;

Toolchain::Toolchain()
{
}

Toolchain::~Toolchain()
{
}

std::vector<std::string> Toolchain::
executablePaths() const
{
    return { _path + "/usr/bin" };
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
    auto result = plist::Format::Any::Read(settingsFileName);
    if (result.first == nullptr) {
        return nullptr;
    }

    plist::Dictionary *plist = plist::CastTo<plist::Dictionary>(result.first.get());
    if (plist == nullptr) {
        return nullptr;
    }

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

    return toolchain;
}

std::string Toolchain::
DefaultIdentifier(void)
{
    return "com.apple.dt.toolchain.XcodeDefault";
}

