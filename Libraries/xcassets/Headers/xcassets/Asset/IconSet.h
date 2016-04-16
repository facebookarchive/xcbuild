/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_IconSet_h
#define __xcassets_Asset_IconSet_h

#include <xcassets/Asset/Asset.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class IconSet : public Asset {
private:
    // TODO images

private:
    friend class Asset;
    using Asset::Asset;

public:
    // TODO images

public:
    static AssetType Type()
    { return AssetType::IconSet; }
    virtual AssetType type()
    { return AssetType::IconSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("iconset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_IconSet_h

