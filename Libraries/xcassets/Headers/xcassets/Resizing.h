/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Resizing_h
#define __xcassets_Resizing_h

#include <xcassets/Insets.h>
#include <plist/Dictionary.h>

#include <ext/optional>

namespace xcassets {

/*
 * Resizing information for a stretchable image.
 */
class Resizing {
public:
    class Center {
    public:
        enum class Mode {
            Tile,
            Stretch,
        };

    private:
        ext::optional<Mode>   _mode;
        ext::optional<double> _width;
        ext::optional<double> _height;

    public:
        ext::optional<Mode> const &mode() const
        { return _mode; }
        ext::optional<double> const &width() const
        { return _width; }
        ext::optional<double> const &height() const
        { return _height; }

    public:
        bool parse(plist::Dictionary const *dict);
    };

public:
    enum class Mode {
        ThreePartHorizontal,
        ThreePartVertical,
        NinePart,
    };

private:
    ext::optional<Mode>   _mode;
    ext::optional<Center> _center;
    ext::optional<Insets> _capInsets;

public:
    ext::optional<Mode> const &mode() const
    { return _mode; }
    ext::optional<Center> const &center() const
    { return _center; }
    ext::optional<Insets>const &capInsets() const
    { return _capInsets; }

public:
    bool parse(plist::Dictionary const *dict);
};

}

#endif // !__xcassets_Resizing_h
