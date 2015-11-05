/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxsetting/Type.h>

using pbxsetting::Type;
using libutil::Wildcard;

Type::
Type()
{
}

Type::
~Type()
{
}

bool Type::
ParseBoolean(std::string const &value)
{
    return (!value.empty() && value != "NO");
}

static std::string
FindClosing(std::string const &value, std::string::size_type offset, char end)
{
    while (true) {
        size_t pos = value.find(end, offset);
        if (pos == std::string::npos) {
            return value.substr(offset);
        }

        if (value[pos - 1] == '\\') {
            // TODO(grp): Escaped quotes aren't handled right at all.
            offset = pos + 1;
        } else {
            return value.substr(offset, pos - offset);
        }
    }
}

static std::string::size_type
FindSpace(std::string const &value, std::string::size_type offset)
{
    size_t space = offset;
    while (space < value.size() && !::isspace(value[space])) {
        ++space;
    }
    if (space == value.size()) {
        space = std::string::npos;
    }
    return space;
}

std::vector<std::string> Type::
ParseList(std::string const &value)
{
    std::vector<std::string> entries;

    size_t offset = 0;
    while (true) {
        size_t singleq = value.find('\'', offset);
        size_t doubleq = value.find('"', offset);
        size_t spaceq = FindSpace(value, offset);

        char end = '\0';
        std::string::size_type quote;

        if (singleq != std::string::npos &&
            (doubleq == std::string::npos || singleq < doubleq) &&
            (spaceq == std::string::npos || singleq < spaceq)) {
            end = '\'';
            quote = singleq;
        } else if (doubleq != std::string::npos &&
            (singleq == std::string::npos || doubleq < singleq) &&
            (spaceq == std::string::npos || doubleq < spaceq)) {
            end = '"';
            quote = doubleq;
        }

        std::string str;

        if (end != '\0') {
            str += value.substr(offset, quote - offset);
            std::string substr = FindClosing(value, quote + 1, end);
            str += substr;

            offset = quote + 1 + substr.size() + 1;
            spaceq = FindSpace(value, offset);
        }

        if (spaceq != std::string::npos) {
            str += value.substr(offset, spaceq - offset);
            if (!str.empty()) {
                entries.push_back(str);
            }

            offset = spaceq;
            while (offset < value.size() && ::isspace(value[offset])) {
                ++offset;
            }
        } else {
            str += value.substr(offset);
            if (!str.empty()) {
                entries.push_back(str);
            }
            break;
        }
    }

    return entries;
}
