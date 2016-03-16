/* Copyright 2013-present Facebook. All Rights Reserved. */

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

