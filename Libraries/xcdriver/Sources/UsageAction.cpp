/**
 Copyright (c) 2016-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/UsageAction.h>
#include <xcdriver/Usage.h>

#include <cstdio>

using xcdriver::UsageAction;
using xcdriver::Usage;

UsageAction::
UsageAction()
{
}

UsageAction::
~UsageAction()
{
}

int UsageAction::
Run()
{
    fprintf(stdout, "%s", Usage::Text().c_str());

    return 0;
}
