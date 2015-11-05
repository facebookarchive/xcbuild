/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Crypto.h>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

using libutil::Crypto;

bool Crypto::
GetRandom(size_t count, void *buffer)
{
    int fd = ::open("/dev/random", O_RDONLY);
    if (fd < 0) {
        fd = ::open("/dev/urandom", O_RDONLY);
        if (fd < 0)
            return false;
    }

    ::read(fd, buffer, count);
    ::close(fd);

    return true;
}
