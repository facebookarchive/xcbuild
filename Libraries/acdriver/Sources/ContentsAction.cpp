/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/ContentsAction.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>

using acdriver::ContentsAction;
using acdriver::Options;
using acdriver::Output;
using acdriver::Result;

ContentsAction::
ContentsAction()
{
}

ContentsAction::
~ContentsAction()
{
}

void ContentsAction::
run(Options const &options, Output *output, Result *result)
{
    // TODO: Implement contents action.
    result->normal(Result::Severity::Warning, "contents not yet implemented");
}

