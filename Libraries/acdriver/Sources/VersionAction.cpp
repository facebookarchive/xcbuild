/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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

