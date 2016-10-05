/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcassets_Asset_Stickers_h
#define __xcassets_Asset_Stickers_h

#include <xcassets/Asset/Asset.h>

namespace xcassets {
namespace Asset {

class Stickers : public Asset {
private:
    std::vector<std::shared_ptr<Asset>> _children;

private:
    friend class Asset;
    using Asset::Asset;

public:
    std::vector<std::shared_ptr<Asset>> const &children() const
    { return _children; }

public:
    static AssetType Type()
    { return AssetType::Stickers; }
    virtual AssetType type()
    { return AssetType::Stickers; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("xcstickers"); }

public:
    /*
     * Load an sticker catalog from a directory.
     */
    static std::shared_ptr<Stickers> Load(libutil::Filesystem const *filesystem, std::string const &path);

protected:
    virtual bool load(libutil::Filesystem const *filesystem);
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_Stickers_h

