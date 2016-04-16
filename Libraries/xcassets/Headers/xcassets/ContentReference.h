/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_ContentReference_h
#define __xcassets_ContentReference_h

#include <xcassets/FullyQualifiedName.h>
#include <xcassets/MatchingStyle.h>
#include <plist/Dictionary.h>

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * A reference to another piece of content.
 */
class ContentReference {
private:
    ext::optional<std::string>        _type;
    ext::optional<MatchingStyle>      _matchingStyle;
    ext::optional<FullyQualifiedName> _name;

public:
    ext::optional<std::string> const &type() const
    { return _type; }
    ext::optional<MatchingStyle> const &matchingStyle() const
    { return _matchingStyle; }
    ext::optional<FullyQualifiedName> const &name() const
    { return _name; }

public:
    bool parse(plist::Dictionary const *dict);
};

}

#endif // !__xcassets_ContentReference_h
