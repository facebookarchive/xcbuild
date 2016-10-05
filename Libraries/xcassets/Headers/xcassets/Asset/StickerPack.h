/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcassets_Asset_StickerPack_h
#define __xcassets_Asset_StickerPack_h

#include <xcassets/Asset/Asset.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class StickerPack : public Asset {
public:
    /*
     * The sizes to display stickers to pick.
     */
    enum class GridSize {
        Small,
        Regular,
        Large,
    };

public:
    class GridSizes {
    private:
        GridSizes();
        ~GridSizes();

    public:
        /*
         * Parse a grid size string.
         */
        static ext::optional<GridSize> Parse(std::string const &value);

        /*
         * String representation of a grid size.
         */
        static std::string String(GridSize value);
    };

private:
    ext::optional<GridSize>             _gridSize;

private:
    // TODO: stickers
    std::vector<std::shared_ptr<Asset>> _children;

private:
    friend class Asset;
    using Asset::Asset;

public:
    ext::optional<GridSize> const &gridSize() const
    { return _gridSize; }

public:
    // TODO: stickers
    std::vector<std::shared_ptr<Asset>> const &children() const
    { return _children; }

public:
    static AssetType Type()
    { return AssetType::StickerPack; }
    virtual AssetType type()
    { return AssetType::StickerPack; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("stickerpack"); }

protected:
    virtual bool load(libutil::Filesystem const *filesystem);
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_StickerPack_h
