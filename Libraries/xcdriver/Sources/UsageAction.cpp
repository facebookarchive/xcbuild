/**
 Copyright (c) 2016-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/UsageAction.h>
#include <xcdriver/Usage.h>
#include <libutil/FSUtil.h>
#include <libutil/SysUtil.h>

using xcdriver::UsageAction;
using xcdriver::usage;
using libutil::FSUtil;
using libutil::SysUtil;

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
    fprintf(stdout, "%s", usage().c_str());
    return 0;
}

