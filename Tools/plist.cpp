// Copyright 2013-present Facebook. All Rights Reserved.

#include <plist/plist.h>

#include <cstring>
#include <cerrno>

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.plist\n", argv[0]);
        return -1;
    }

    auto X = plist::Object::Parse(argv[1],
            [&](unsigned line, unsigned column, std::string const &message)
            {
                if (line != 0 && column != 0) {
                    fprintf(stderr, "%s:%d.%d:error: %s\n", argv[1],
                        line, column, message.c_str());
                } else {
                    fprintf(stderr, "%s:error: %s\n", argv[1],
                        message.c_str());
                }

                exit(EXIT_FAILURE);
            });

    X->dump();
    X->release();

    return 0;
}
