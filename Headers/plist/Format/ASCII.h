/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_ASCII_h
#define __plist_Format_ASCII_h

#include <plist/Format/Base.h>
#include <plist/Format/Encoding.h>

namespace plist {
namespace Format {

class ASCII : public Base<ASCII> {
private:
    Encoding _encoding;

private:
    ASCII(Encoding encoding);

public:
    inline Encoding encoding() const
    { return _encoding; }

public:
    static ASCII Create(Encoding encoding);
};

}
}

#endif  // !__plist_Format_ASCII_h
