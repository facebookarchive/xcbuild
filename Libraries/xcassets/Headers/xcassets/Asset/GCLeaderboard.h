/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_GCLeaderboard_h
#define __xcassets_Asset_GCLeaderboard_h

#include <xcassets/Asset/Asset.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class ImageStack;

class GCLeaderboard : public Asset {
private:
    std::vector<std::shared_ptr<ImageStack>> _children;

public:
    // TODO: content-reference
    std::vector<std::shared_ptr<ImageStack>> children() const
    { return _children; }

public:
    static AssetType Type()
    { return AssetType::GCLeaderboard; }
    virtual AssetType type()
    { return AssetType::GCLeaderboard; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("gcleaderboard"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}
}

#endif // !__xcassets_Asset_GCLeaderboard_h
