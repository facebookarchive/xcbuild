/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_LaunchImage_h
#define __xcassets_LaunchImage_h

#include <xcassets/Asset.h>
#include <xcassets/DeviceSubtype.h>
#include <xcassets/Idiom.h>
#include <xcassets/Orientation.h>
#include <xcassets/Scale.h>
#include <plist/Dictionary.h>

#include <string>
#include <vector>
#include <ext/optional>

namespace xcassets {

class LaunchImage : public Asset {
public:
    class Image {
    public:
        /*
         * The visual extent of a launch image.
         */
        enum class Extent {
            ToStatusBar,
            FullScreen,
        };

        class Extents {
        private:
            Extents();
            ~Extents();

        public:
            /*
             * Parse an extent from a string.
             */
            static ext::optional<Extent> Parse(std::string const &value);

            /*
             * Convert an extent to a string.
             */
            static std::string String(Extent extent);
        };

    private:
        ext::optional<std::string>   _fileName;
        ext::optional<Idiom>         _idiom;
        ext::optional<Orientation>   _orientation;
        ext::optional<double>        _scale;
        ext::optional<DeviceSubtype> _subtype;
        // TODO: minimum-system-version
        ext::optional<Extent>        _extent;

    public:
        ext::optional<std::string> const &fileName() const
        { return _fileName; }
        ext::optional<Idiom> const &idiom() const
        { return _idiom; }
        ext::optional<Orientation> const &orientation() const
        { return _orientation; }
        ext::optional<double> const &scale() const
        { return _scale; }
        ext::optional<DeviceSubtype> const &subtype() const
        { return _subtype; }
        // TODO: minimum-system-version
        ext::optional<Extent> const &extent() const
        { return _extent; }

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
