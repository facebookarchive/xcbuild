/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_LaunchImage_h
#define __xcassets_LaunchImage_h

#include <xcassets/Asset.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class LaunchImage : public Asset {
private:
    // TODO data

public:
    // TODO data

public:
    static AssetType Type()
    { return AssetType::LaunchImage; }
    virtual AssetType type()
    { return AssetType::LaunchImage; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("launchimage"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}

#endif // !__xcassets_LaunchImage_h
