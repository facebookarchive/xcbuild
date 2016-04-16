/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/CompileAction.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>

using acdriver::CompileAction;
using acdriver::Options;
using acdriver::Output;
using acdriver::Result;

CompileAction::
CompileAction()
{
}

CompileAction::
~CompileAction()
{
}

void CompileAction::
run(Options const &options, Output *output, Result *result)
{
    // TODO: Implement compile action.
    result->normal(Result::Severity::Warning, "compile not yet implemented");
}

