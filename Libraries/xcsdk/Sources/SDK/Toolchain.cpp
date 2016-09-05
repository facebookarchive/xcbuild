/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/SDK/Toolchain.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <plist/Format/Any.h>

using xcsdk::SDK::Toolchain;
using libutil::Filesystem;
using libutil::FSUtil;

Toolchain::
Toolchain()
{
}

Toolchain::
~Toolchain()
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
Open(Filesystem const *filesystem, std::shared_ptr<Manager> manager, std::string const &path)
{
    if (path.empty()) {
        return nullptr;
    }

    std::string settingsFileName = path + "/ToolchainInfo.plist";
    if (!filesystem->isReadable(settingsFileName)) {
        return nullptr;
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
    // Parse the toolchain dictionary and create the object.
    //
    auto toolchain = std::make_shared<Toolchain>();
    if (!toolchain->parse(plist)) {
        return nullptr;
    }

    //
    // Save some useful info
    //
    toolchain->_path = FSUtil::GetDirectoryName(realPath);
    toolchain->_name = FSUtil::GetBaseNameWithoutExtension(toolchain->_path);

    return toolchain;
}

std::string Toolchain::
DefaultIdentifier(void)
{
    return "com.apple.dt.toolchain.XcodeDefault";
}

