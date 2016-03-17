/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_AppIconSet_h
#define __xcassets_AppIconSet_h

#include <xcassets/Asset.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class AppIconSet : public Asset {
private:
    // TODO: pre-rendered
    // TODO: images

public:
    // TODO: pre-rendered
    // TODO: images

public:
    static AssetType Type()
    { return AssetType::AppIconSet; }
    virtual AssetType type()
    { return AssetType::AppIconSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("appiconset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}

#endif // !__xcassets_AppIconSet_h
