/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_ImageStackLayer_h
#define __xcassets_Asset_ImageStackLayer_h

#include <xcassets/Asset/Asset.h>
#include <xcassets/ContentReference.h>
#include <plist/Dictionary.h>

#include <string>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class ImageStackLayer : public Asset {
private:
    // TODO: content (image set)
    ext::optional<ContentReference> _contentReference;
    // TODO: content-reference
    // TODO: frame-size
    // TODO: frame-center

private:
    friend class Asset;
    using Asset::Asset;

public:
    // TODO: content (image set)
    ext::optional<ContentReference> const &contentReference() const
    { return _contentReference; }
    // TODO: frame-size
    // TODO: frame-center

public:
    static AssetType Type()
    { return AssetType::ImageStackLayer; }
    virtual AssetType type()
    { return AssetType::ImageStackLayer; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("imagestacklayer"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_ImageStackLayer_h

