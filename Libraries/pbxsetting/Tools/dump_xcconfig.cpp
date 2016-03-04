/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxsetting/XC/Config.h>
#include <pbxsetting/Environment.h>
#include <pbxsetting/Setting.h>
#include <libutil/FSUtil.h>

#include <cstdio>

using pbxsetting::XC::Config;
using pbxsetting::Environment;
using pbxsetting::Setting;
using libutil::FSUtil;

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.xcconfig\n", argv[0]);
        return -1;
    }

    auto environment = Environment::Empty();
    auto config = Config::Open(argv[1], environment,
            [](std::string const &filename, unsigned line,
                std::string const &message) -> bool
            {
                fprintf(stderr, "%s line %u: %s\n", filename.c_str(), line,
                    message.c_str());
                return true; // Ignore error and continue.
            });

    if (!config) {
        fprintf(stderr, "The file \"%s\" couldn’t be opened "
                "because its path couldn't be resolved.  "
                "It may be missing.\n", argv[1]);
        return -1;
    }

    for (Setting const &setting : config->level().settings()) {
        printf("%s = %s\n", setting.name().c_str(), setting.value().raw().c_str());
    }

    return 0;
}
