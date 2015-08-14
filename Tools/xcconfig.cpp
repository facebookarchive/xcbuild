// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/pbxproj.h>

#include <cstdio>

using pbxproj::XC::Config;
using libutil::FSUtil;

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.xcconfig\n", argv[0]);
        return -1;
    }

    auto config = Config::Open(argv[1],
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
    
    config->settings()->dump();

    return 0;
}
