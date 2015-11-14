/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_StringsParser_h
#define __plist_StringsParser_h

#include <plist/Object.h>

namespace plist {

class StringsParser {
public:
    StringsParser();

public:
    Object *parse(std::string const &path, error_function const &error);
    Object *parse(std::FILE *fp, error_function const &error);
};

}

#endif  // !__plist_StringsParser_h
