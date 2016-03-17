/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_DataSet_h
#define __xcassets_DataSet_h

#include <xcassets/Asset.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class DataSet : public Asset {
private:
    ext::optional<std::vector<std::string>> _onDemandResourceTags;

private:
    // TODO data

public:
    ext::optional<std::vector<std::string>> const &onDemandResourceTags() const
    { return _onDemandResourceTags; }

public:
    // TODO data

public:
    static AssetType Type()
    { return AssetType::DataSet; }
    virtual AssetType type()
    { return AssetType::DataSet; }

public:
    static ext::optional<std::string> Extension()
    { return std::string("dataset"); }

protected:
    virtual bool parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check);
};

}

#endif // !__xcassets_DataSet_h
