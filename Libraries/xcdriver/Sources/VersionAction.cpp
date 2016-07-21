/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/VersionAction.h>
#include <xcdriver/Options.h>
#include <xcsdk/Configuration.h>
#include <xcsdk/Environment.h>
#include <xcsdk/SDK/Manager.h>
#include <xcsdk/SDK/Platform.h>
#include <xcsdk/SDK/PlatformVersion.h>
#include <xcsdk/SDK/Product.h>
#include <xcsdk/SDK/Target.h>
#include <libutil/Filesystem.h>

using xcdriver::VersionAction;
using xcdriver::Options;
using libutil::Filesystem;

VersionAction::
VersionAction()
{
}

VersionAction::
~VersionAction()
{
}

int VersionAction::
Run(Filesystem const *filesystem, Options const &options)
{
    if (!options.sdk()) {
        // TODO(grp): Real version numbers.
        printf("xcbuild version 0.1\n");
        printf("Build version 1\n");
    } else {
        ext::optional<std::string> developerRoot = xcsdk::Environment::DeveloperRoot(filesystem);
        if (!developerRoot) {
            fprintf(stderr, "error: unable to find developer dir\n");
            return 1;
        }

        auto configuration = xcsdk::Configuration::Load(filesystem, xcsdk::Configuration::DefaultPaths());
        auto manager = xcsdk::SDK::Manager::Open(filesystem, *developerRoot, configuration);
        if (manager == nullptr) {
            fprintf(stderr, "error: unable to open developer directory\n");
            return 1;
        }

        xcsdk::SDK::Target::shared_ptr target = manager->findTarget(*options.sdk());
        if (target == nullptr) {
            fprintf(stderr, "error: cannot find sdk '%s'\n", options.sdk()->c_str());
            return 1;
        }

        printf("%s - %s (%s)\n",
                target->bundleName().c_str(),
                target->displayName().c_str(),
                target->canonicalName().c_str());
        if (!target->version().empty()) {
            printf("SDKVersion: %s\n", target->version().c_str());
        }
        printf("Path: %s\n", target->path().c_str());
        if (!target->platform()->version().empty()) {
            printf("PlatformVersion: %s\n", target->platform()->version().c_str());
        }
        printf("PlatformPath: %s\n", target->platform()->path().c_str());
        if (auto product = target->product()) {
            if (!product->buildVersion().empty()) {
                printf("ProductBuildVersion: %s\n", product->buildVersion().c_str());
            }
            if (!product->copyright().empty()) {
                printf("ProductCopyright: %s\n", product->copyright().c_str());
            }
            if (!product->name().empty()) {
                printf("ProductName: %s\n", product->name().c_str());
            }
            if (!product->userVisibleVersion().empty()) {
                printf("ProductUserVisibleVersion: %s\n", product->userVisibleVersion().c_str());
            }
            if (!product->version().empty()) {
                printf("ProductVersion: %s\n", product->version().c_str());
            }
        }
        printf("\n");
    }

    return 0;
}
