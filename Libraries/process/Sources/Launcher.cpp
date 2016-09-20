/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/Launcher.h>

using process::Launcher;

Launcher::
Launcher()
{
}

Launcher::
~Launcher()
{
}

#include <process/DefaultLauncher.h>

using process::DefaultLauncher;

Launcher *Launcher::
GetDefaultUNSAFE()
{
    static DefaultLauncher *defaultLauncher = nullptr;
    if (defaultLauncher == nullptr) {
        defaultLauncher = new DefaultLauncher();
    }

    return defaultLauncher;
}

