/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_ImageSet_h
#define __xcassets_ImageSet_h

#include <xcassets/Asset.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class ImageSet : public Asset {
private:
    ext::optional<std::vector<std::string>> _onDemandResourceTags;

private:
    // TODO images

public:
    ext::optional<std::vector<std::string>> const &onDemandResourceTags() const
    { return _onDemandResourceTags; }

public:
    // TODO images

public:
    static AssetType Type()
    { return AssetType::ImageSet; }
    virtual AssetType type()
    { return AssetType::ImageSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("imageset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}

#endif // !__xcassets_ImageSet_h
