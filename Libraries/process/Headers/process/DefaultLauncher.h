/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#ifndef __process_DefaultLauncher_h
#define __process_DefaultLauncher_h

#include <process/Launcher.h>

namespace libutil { class Filesystem; }

namespace process {

/*
 * Abstract process launcher.
 */
class DefaultLauncher : public Launcher {
public:
    DefaultLauncher();
    ~DefaultLauncher();

public:
    virtual ext::optional<int> launch(libutil::Filesystem *filesystem, Context const *context);
};

}

#endif  // !__process_DefaultLauncher_h

