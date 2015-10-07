// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcdriver/Driver.h>
#include <xcdriver/Action.h>
#include <xcdriver/Options.h>
#include <xcdriver/ShowSDKsAction.h>

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
    auto opt = Options::Parse(args);
    if (!opt.second.empty()) {
        fprintf(stderr, "error: %s\n", opt.second.c_str());
        return 1;
    }

    Options const &options = opt.first;
    Action::Type action = Action::Determine(options);
    switch (action) {
        case Action::Build:
            fprintf(stderr, "warning: build not implemented\n");
            break;
        case Action::Version:
            fprintf(stderr, "warning: version not implemented\n");
            break;
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
            fprintf(stderr, "warning: find not implemented\n");
            break;
        case Action::ExportArchive:
            fprintf(stderr, "warning: export archive not implemented\n");
            break;
        case Action::Localizations:
            fprintf(stderr, "warning: localizations not implemented\n");
            break;
    }

    return 0;
}
