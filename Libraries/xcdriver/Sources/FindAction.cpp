/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/FindAction.h>
#include <xcdriver/Options.h>
#include <xcsdk/xcsdk.h>
#include <libutil/DefaultFilesystem.h>
#include <libutil/Filesystem.h>

using xcdriver::FindAction;
using xcdriver::Options;
using libutil::DefaultFilesystem;
using libutil::Filesystem;

FindAction::
FindAction()
{
}

FindAction::
~FindAction()
{
}

int FindAction::
Run(Options const &options)
{
    std::unique_ptr<Filesystem> filesystem = std::unique_ptr<Filesystem>(new DefaultFilesystem());

    ext::optional<std::string> developerRoot = xcsdk::Environment::DeveloperRoot(filesystem.get());
    if (!developerRoot) {
        fprintf(stderr, "error: unable to find developer dir\n");
        return 1;
    }

    std::shared_ptr<xcsdk::SDK::Manager> manager = xcsdk::SDK::Manager::Open(filesystem.get(), *developerRoot);
    if (manager == nullptr) {
        fprintf(stderr, "error: unable to open developer directory\n");
        return 1;
    }

    std::string sdk = options.sdk();
    if (sdk.empty()) {
        sdk = "macosx";
    }

    xcsdk::SDK::Target::shared_ptr target = manager->findTarget(sdk);
    if (target == nullptr) {
        fprintf(stderr, "error: cannot find sdk '%s'\n", sdk.c_str());
        return 1;
    }

    if (!options.findExecutable().empty()) {
        ext::optional<std::string> executable = filesystem->findExecutable(options.findExecutable(), target->executablePaths());
        if (!executable) {
            fprintf(stderr, "error: '%s' not found\n", options.findExecutable().c_str());
            return 1;
        }

        printf("%s\n", executable->c_str());
        return 0;
    } else if (!options.findLibrary().empty()) {
        fprintf(stderr, "warning: finding libraries is not supported\n");
        return 0;
    } else {
        fprintf(stderr, "error: unknown find option\n");
        return 1;
    }
}
