/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/Driver.h>
#include <xcdriver/Action.h>
#include <xcdriver/Options.h>
#include <xcdriver/BuildAction.h>
#include <xcdriver/FindAction.h>
#include <xcdriver/ListAction.h>
#include <xcdriver/ShowSDKsAction.h>
#include <xcdriver/ShowBuildSettingsAction.h>
#include <xcdriver/VersionAction.h>

using xcdriver::Driver;
using xcdriver::Action;
using xcdriver::Options;

Driver::
Driver()
{
}

Driver::
~Driver()
{
}

int Driver::
Run(std::vector<std::string> const &args)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    Action::Type action = Action::Determine(options);
    switch (action) {
        case Action::Build:
            return BuildAction::Run(options);
        case Action::ShowBuildSettings:
            return ShowBuildSettingsAction::Run(options);
        case Action::List:
            return ListAction::Run(options);
        case Action::Version:
            return VersionAction::Run(options);
        case Action::Usage:
            fprintf(stderr, "warning: usage not implemented\n");
            break;
        case Action::Help:
            fprintf(stderr, "warning: help not implemented\n");
            break;
        case Action::License:
            fprintf(stderr, "warning: license not implemented\n");
            break;
        case Action::CheckFirstLaunch:
            fprintf(stderr, "warning: check first launch not implemented\n");
            break;
        case Action::ShowSDKs:
            return ShowSDKsAction::Run(options);
        case Action::Find:
            return FindAction::Run(options);
        case Action::ExportArchive:
            fprintf(stderr, "warning: export archive not implemented\n");
            break;
        case Action::Localizations:
            fprintf(stderr, "warning: localizations not implemented\n");
            break;
    }

    return 0;
}
