// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/AnalyzeAction.h>

using xcscheme::XC::AnalyzeAction;

AnalyzeAction::AnalyzeAction()
{
}

bool AnalyzeAction::
parse(plist::Dictionary const *dict)
{
    if (!Action::parse(dict))
        return false;

    return true;
}
