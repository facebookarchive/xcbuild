/* Copyright 2013-present Facebook. All Rights Reserved. */

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
    std::vector<std::shared_ptr<ImageSet>>  _children;

private:
    // TODO: assets

private:
    friend class Asset;
    using Asset::Asset;

public:
    std::vector<std::shared_ptr<ImageSet>> children() const
    { return _children; }

public:
    // TODO: assets

public:
    static AssetType Type()
    { return AssetType::ComplicationSet; }
    virtual AssetType type()
    { return AssetType::ComplicationSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("complicationset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_ComplicationSet_h
