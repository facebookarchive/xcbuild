/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/ShowSDKsAction.h>
#include <xcdriver/Options.h>

#include <xcsdk/xcsdk.h>

using xcdriver::ShowSDKsAction;
using xcdriver::Options;

ShowSDKsAction::
ShowSDKsAction()
{
}

ShowSDKsAction::
~ShowSDKsAction()
{
}

int ShowSDKsAction::
Run(Options const &options)
{
    std::string developerRoot = xcsdk::Environment::DeveloperRoot();
    std::shared_ptr<xcsdk::SDK::Manager> manager = xcsdk::SDK::Manager::Open(developerRoot);
    if (manager == nullptr) {
        fprintf(stderr, "error: unable to open developer directory\n");
        return 1;
    }

    for (auto const &platform : manager->platforms()) {
        printf("%s SDKs:\n", platform->description().c_str());
        for (auto const &target : platform->targets()) {
            printf("\t%-32s-sdk %s\n", target->displayName().c_str(), target->canonicalName().c_str());
        }
        printf("\n");
    }

    return 0;
}
