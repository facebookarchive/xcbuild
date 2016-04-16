/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Asset_AppIconSet_h
#define __xcassets_Asset_AppIconSet_h

#include <xcassets/Asset/Asset.h>
#include <xcassets/Slot/Idiom.h>
#include <xcassets/Slot/Scale.h>
#include <xcassets/Slot/WatchIconRole.h>
#include <xcassets/MatchingStyle.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {
namespace Asset {

class AppIconSet : public Asset {
public:
    class Image {
    private:
        ext::optional<std::string>         _fileName;
        ext::optional<bool>                _unassigned;
        ext::optional<MatchingStyle>       _matchingStyle;

    private:
        ext::optional<Slot::Idiom>         _idiom;
        // TODO: size
        ext::optional<Slot::Scale>         _scale;
        ext::optional<Slot::WatchIconRole> _role;
        // TODO: subtype (watch)

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
        ext::optional<Slot::Idiom> const &idiom() const
        { return _idiom; }
        // TODO: size
        ext::optional<Slot::Scale> const &scale() const
        { return _scale; }
        ext::optional<Slot::WatchIconRole> const &role() const
        { return _role; }
        // TODO: subtype (watch)

    private:
        friend class AppIconSet;
        bool parse(plist::Dictionary const *dict);
    };

private:
    ext::optional<bool>               _preRendered;
    ext::optional<std::vector<Image>> _images;

private:
    friend class Asset;
    using Asset::Asset;

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
}

#endif // !__xcassets_Asset_AppIconSet_h
