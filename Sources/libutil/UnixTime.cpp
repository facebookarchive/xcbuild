// Copyright 2013-present Facebook. All Rights Reserved.

#include <libutil/UnixTime.h>

using libutil::UnixTime;

void UnixTime::
Decode(uint64_t in, struct tm &out)
{
    time_t t = in;
    ::gmtime_r(&t, &out);
}

uint64_t UnixTime::
Encode(struct tm const &in)
{
    struct tm copy = in;
    return ::mktime(&copy);
}
