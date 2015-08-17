// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>

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

    auto environment = Environment({ }, { });
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
