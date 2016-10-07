/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcassets_Asset_TextureSet_h
#define __xcassets_Asset_TextureSet_h

#include <xcassets/Asset/Asset.h>
#include <xcassets/TextureInterpretation.h>
#include <xcassets/TextureOrigin.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class MipmapSet;

class TextureSet : public Asset {
private:
    ext::optional<TextureInterpretation>    _interpretation;
    ext::optional<TextureOrigin>            _origin;
    ext::optional<std::vector<std::string>> _onDemandResourceTags;
    // TODO: textures

private:
    friend class Asset;
    using Asset::Asset;

public:
    ext::optional<TextureInterpretation> const &interpretation() const
    { return _interpretation; }
    ext::optional<TextureOrigin> const &origin() const
    { return _origin; }
    ext::optional<std::vector<std::string>> const &onDemandResourceTags() const
    { return _onDemandResourceTags; }
    // TODO: textures

public:
    static AssetType Type()
    { return AssetType::TextureSet; }
    virtual AssetType type() const
    { return AssetType::TextureSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("textureset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_TextureSet_h
