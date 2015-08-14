// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/pbxbuild.h>

using pbxbuild::HeaderMap;

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.hmap\n", argv[0]);
        return -1;
    }

    HeaderMap hmap;
    if (!hmap.read(argv[1])) {
        fprintf(stderr, "error: cannot open '%s', either non-existant or "
                "not an hmap file\n", argv[1]);
        return -1;
    }

    hmap.dump();

    return 0;
}
