// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/AnalyzeAction.h>

using pbxproj::XC::AnalyzeAction;

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
