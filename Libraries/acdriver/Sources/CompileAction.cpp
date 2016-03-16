/* Copyright 2013-present Facebook. All Rights Reserved. */

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

