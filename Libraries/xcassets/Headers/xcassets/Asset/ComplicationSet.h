/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcassets_Asset_ComplicationSet_h
#define __xcassets_Asset_ComplicationSet_h

#include <xcassets/Asset/Asset.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class ImageSet;

class ComplicationSet : public Asset {
private:
    std::vector<std::unique_ptr<ImageSet>>  _children;

private:
    // TODO: assets

private:
    friend class Asset;
    using Asset::Asset;

public:
    std::vector<std::unique_ptr<ImageSet>> const &children() const
    { return _children; }

public:
    // TODO: assets

public:
    static AssetType Type()
    { return AssetType::ComplicationSet; }
    virtual AssetType type() const
    { return AssetType::ComplicationSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("complicationset"); }

protected:
    virtual bool load(libutil::Filesystem const *filesystem);
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_ComplicationSet_h
