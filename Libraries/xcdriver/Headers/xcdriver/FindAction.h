/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcdriver_FindAction_h
#define __xcdriver_FindAction_h

#include <xcdriver/Base.h>

namespace libutil { class Filesystem; }

namespace xcdriver {

class Options;

class FindAction {
private:
    FindAction();
    ~FindAction();

public:
    static int
    Run(libutil::Filesystem const *filesystem, Options const &options);
};

}

#endif // !__xcdriver_FindAction_h
