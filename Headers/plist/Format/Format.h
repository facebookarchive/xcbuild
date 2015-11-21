/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_Format_h
#define __plist_Format_Format_h

#include <plist/Format/Info.h>

namespace plist {
namespace Format {

class Format {
private:
    Format();
    ~Format();

public:
    static std::unique_ptr<Info>
    Identify(std::vector<uint8_t> const &contents);

public:
    static std::pair<Object *, std::string>
    Deserialize(std::vector<uint8_t> const &contents, Info const &info);

public:
    static std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string>
    Serialize(Object *object, Info const &info);
};

}
}

#endif  // !__plist_Format_Format_h
