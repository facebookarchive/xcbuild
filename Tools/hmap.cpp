/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/pbxbuild.h>

#include <iterator>
#include <fstream>

using pbxbuild::HeaderMap;

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.hmap\n", argv[0]);
        return -1;
    }

    std::ifstream file = std::ifstream(argv[1], std::ios::binary);
    std::vector<char> contents = std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    HeaderMap hmap;
    if (!hmap.read(contents)) {
        fprintf(stderr, "error: cannot open '%s', either non-existant or "
                "not an hmap file\n", argv[1]);
        return -1;
    }

    hmap.dump();

    return 0;
}
