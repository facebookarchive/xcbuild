/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_FullyQualifiedName_h
#define __xcassets_FullyQualifiedName_h

#include <string>
#include <vector>

namespace xcassets {

/*
 * The unique identifier of an asset in a catalog.
 */
class FullyQualifiedName {
private:
    std::vector<std::string> _groups;
    std::string              _name;

public:
    FullyQualifiedName(std::vector<std::string> const &groups, std::string const &name);

public:
    /*
     * The list of groups containing this asset.
     */
    std::vector<std::string> const &groups() const
    { return _groups; }

    /*
     * The name of the asset.
     */
    std::string const &name() const
    { return _name; }

public:
    /*
     * The string representation of the asset.
     */
    std::string string() const;

    /*
     * Parse a fully qualified name from a string.
     */
    static FullyQualifiedName Parse(std::string const &string);
};

}

#endif // !__xcassets_FullyQualifiedName_h
