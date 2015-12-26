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
    return strcasecmp(value.c_str(), "yes") == 0 || strcasecmp(value.c_str(), "true") == 0;
}

int64_t Type::
ParseInteger(std::string const &value)
{
    return std::strtoll(value.c_str(), NULL, 0);
}

std::vector<std::string> Type::
ParseList(std::string const &value)
{
    std::vector<std::string> entries;

    std::string str;
    char quote = '\0';
    bool escaped = false;

    for (std::string::size_type i = 0; i < value.size(); i++) {
        char c = value[i];
        if (!escaped) {
            if (c == '\'' || c == '"') {
                if (quote != '\0') {
                    if (c == quote) {
                        quote = '\0';
                        continue;
                    }
                } else {
                    quote = c;
                    continue;
                }
            } else if (c == '\\') {
                escaped = true;
                continue;
            } else if (quote == '\0' && isspace(c)) {
                if (!str.empty()) {
                    entries.push_back(str);
                    str = std::string();
                }
                continue;
            }
        }

        str += c;
        escaped = false;
    }

    if (!str.empty()) {
        entries.push_back(str);
        str = std::string();
    }

    return entries;
}
