/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_BrandAssets_h
#define __xcassets_Asset_BrandAssets_h

#include <xcassets/Asset/Asset.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class ImageSet;

class BrandAssets : public Asset {
private:
    // TODO: image set and image stack
    std::vector<std::shared_ptr<Asset>> _children;

private:
    // TODO: assets

private:
    friend class Asset;
    using Asset::Asset;

public:
    // TODO: image set and image stack
    std::vector<std::shared_ptr<Asset>> children() const
    { return _children; }

public:
    // TODO: assets

public:
    static AssetType Type()
    { return AssetType::BrandAssets; }
    virtual AssetType type()
    { return AssetType::BrandAssets; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("brandassets"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_BrandAssets_h

