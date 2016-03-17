/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_LaunchImage_h
#define __xcassets_LaunchImage_h

#include <xcassets/Asset.h>
#include <xcassets/Idiom.h>
#include <xcassets/Orientation.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class LaunchImage : public Asset {
public:
    class Image {
    private:
        ext::optional<std::string> _fileName;
        ext::optional<Idiom>       _idiom;
        ext::optional<Orientation> _orientation;
        // TODO: scale
        // TODO: subtype
        // TODO: minimum-system-version
        // TODO: extent

    public:
        ext::optional<std::string> const &fileName() const
        { return _fileName; }
        ext::optional<Idiom> const &idiom() const
        { return _idiom; }
        ext::optional<Orientation> const &orientation() const
        { return _orientation; }
        // TODO: scale
        // TODO: subtype
        // TODO: minimum-system-version
        // TODO: extent

    private:
        friend class LaunchImage;
        bool parse(plist::Dictionary const *dict);
    };

private:
    ext::optional<std::vector<Image>> _images;

public:
    ext::optional<std::vector<Image>> const &images() const
    { return _images; }

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
