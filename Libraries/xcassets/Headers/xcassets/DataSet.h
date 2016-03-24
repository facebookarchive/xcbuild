/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_DataSet_h
#define __xcassets_DataSet_h

#include <xcassets/Asset.h>
#include <xcassets/Idiom.h>
#include <xcassets/GraphicsFeatureSet.h>
#include <plist/Dictionary.h>

#include <memory>
#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class DataSet : public Asset {
public:
    class Data {
    private:
        ext::optional<std::string>        _fileName;
        ext::optional<Idiom>              _idiom;
        ext::optional<GraphicsFeatureSet> _graphicsFeatureSet;
        // TODO: memory
        ext::optional<std::string>        _UTI;

    public:
        ext::optional<std::string> const &fileName() const
        { return _fileName; }
        ext::optional<Idiom> const &idiom() const
        { return _idiom; }
        ext::optional<GraphicsFeatureSet> const &graphicsFeatureSet() const
        { return _graphicsFeatureSet; }
        // TODO: memory
        ext::optional<std::string> const &UTI() const
        { return _UTI; }

    private:
        friend class DataSet;
        bool parse(plist::Dictionary const *dict);
    };

private:
    ext::optional<std::vector<std::string>> _onDemandResourceTags;
    ext::optional<std::vector<Data>>        _data;

public:
    ext::optional<std::vector<std::string>> const &onDemandResourceTags() const
    { return _onDemandResourceTags; }
    ext::optional<std::vector<Data>> const &data() const
    { return _data; }

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
