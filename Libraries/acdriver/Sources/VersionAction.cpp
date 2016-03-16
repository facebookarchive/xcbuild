/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <acdriver/VersionAction.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>

using acdriver::VersionAction;
using acdriver::Options;
using acdriver::Output;
using acdriver::Result;

VersionAction::
VersionAction()
{
}

VersionAction::
~VersionAction()
{
}

void VersionAction::
run(Options const &options, Output *output, Result *result)
{
    std::unique_ptr<plist::Dictionary> dict = plist::Dictionary::New();
    dict->set("bundle-version", plist::String::New("1"));
    dict->set("short-bundle-version", plist::String::New("1"));

    std::string text;
    text += "bundle-version: 1\n";
    text += "short-bundle-version: 1\n";

    output->add("com.apple.actool.version", std::move(dict), text);
}

