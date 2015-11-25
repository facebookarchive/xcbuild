/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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

    plist::Format::Any format;
    auto deserialize = plist::Format::Any::Read(argv[1], &format);
    if (deserialize.first == nullptr) {
        fprintf(stderr, "error: %s\n", deserialize.second.c_str());
        return -1;
    }

    plist::Format::ASCII out = plist::Format::ASCII::Create(plist::Format::Encoding::UTF8);
    auto serialize = plist::Format::ASCII::Serialize(deserialize.first, out);
    if (serialize.first == nullptr) {
        fprintf(stderr, "error: %s\n", serialize.second.c_str());
        return -1;
    }

    std::copy(serialize.first->begin(), serialize.first->end(), std::ostream_iterator<char>(std::cout));

    deserialize.first->release();

    return 0;
}
