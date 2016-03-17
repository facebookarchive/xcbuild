/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_AppIconSet_h
#define __xcassets_AppIconSet_h

#include <xcassets/Asset.h>
#include <xcassets/Idiom.h>
#include <xcassets/MatchingStyle.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class AppIconSet : public Asset {
public:
    class Image {
    private:
        ext::optional<std::string>   _fileName;
        ext::optional<bool>          _unassigned;
        ext::optional<MatchingStyle> _matchingStyle;

    private:
        ext::optional<Idiom>         _idiom;
        // TODO: size
        // TODO: scale
        // TODO: role

    public:
        ext::optional<std::string> const &fileName() const
        { return _fileName; }
        bool unassigned() const
        { return _unassigned.value_or(false); }
        ext::optional<bool> const &unassignedOptional() const
        { return _unassigned; }
        ext::optional<MatchingStyle> const &matchingStyle() const
        { return _matchingStyle; }

    public:
        ext::optional<Idiom> const &idiom() const
        { return _idiom; }
        // TODO: size
        // TODO: scale
        // TODO: role

    private:
        friend class AppIconSet;
        bool parse(plist::Dictionary const *dict);
    };

private:
    ext::optional<bool>               _preRendered;
    ext::optional<std::vector<Image>> _images;

public:
    bool preRendered() const
    { return _preRendered.value_or(false); }
    ext::optional<bool> const &preRenderedOptional() const
    { return _preRendered; }
    ext::optional<std::vector<Image>> const &images() const
    { return _images; }

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
