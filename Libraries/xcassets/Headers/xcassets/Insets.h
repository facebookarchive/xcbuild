/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Insets_h
#define __xcassets_Insets_h

#include <plist/Dictionary.h>

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * Edge insets are insets from edges on all sides.
 */
class Insets {
private:
    ext::optional<double> _top;
    ext::optional<double> _left;
    ext::optional<double> _bottom;
    ext::optional<double> _right;

public:
    ext::optional<double> const &top() const
    { return _top; }
    ext::optional<double> const &left() const
    { return _left; }
    ext::optional<double> const &bottom() const
    { return _bottom; }
    ext::optional<double> const &right() const
    { return _right; }

public:
    bool parse(plist::Dictionary const *dict);
};

}

#endif // !__xcassets_Insets_h
