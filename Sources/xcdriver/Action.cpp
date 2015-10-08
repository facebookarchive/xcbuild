// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcdriver/Action.h>
#include <xcdriver/Options.h>

using xcdriver::Action;
using xcdriver::Options;

Action::
Action()
{
}

Action::
~Action()
{
}

Action::Type Action::
Determine(Options const &options)
{
    if (options.version()) {
        return Version;
    } else if (options.usage()) {
        return Usage;
    } else if (options.help()) {
        return Help;
    } else if (options.license()) {
        return License;
    } else if (options.checkFirstLaunchStatus()) {
        return CheckFirstLaunch;
    } else if (options.showSDKs()) {
        return ShowSDKs;
    } else if (!options.findLibrary().empty() || !options.findExecutable().empty()) {
        return Find;
    } else if (options.exportArchive()) {
        return ExportArchive;
    } else if (options.exportLocalizations() || options.importLocalizations()) {
        return Localizations;
    } else {
        return Build;
    }
}
