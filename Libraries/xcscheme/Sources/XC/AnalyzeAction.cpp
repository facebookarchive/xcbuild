/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

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
