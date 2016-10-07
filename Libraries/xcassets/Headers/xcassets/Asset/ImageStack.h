/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcassets_Asset_ImageStack_h
#define __xcassets_Asset_ImageStack_h

#include <xcassets/Asset/Asset.h>
#include <xcassets/Asset/ImageStackLayer.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class ImageStack : public Asset {
private:
    std::vector<std::unique_ptr<ImageStackLayer>> _children;

private:
    // TODO layers
    // TODO canvasSize
    ext::optional<std::vector<std::string>>       _onDemandResourceTags;

private:
    friend class Asset;
    using Asset::Asset;

public:
    std::vector<std::unique_ptr<ImageStackLayer>> const &children() const
    { return _children; }

public:
    // TODO layers
    // TODO canvasSize
    ext::optional<std::vector<std::string>> const &onDemandResourceTags() const
    { return _onDemandResourceTags; }

public:
    static AssetType Type()
    { return AssetType::ImageStack; }
    virtual AssetType type() const
    { return AssetType::ImageStack; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("imagestack"); }

protected:
    virtual bool load(libutil::Filesystem const *filesystem);
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_ImageStack_h
