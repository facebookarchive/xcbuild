// Copyright 2013-present Facebook. All Rights Reserved.

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
