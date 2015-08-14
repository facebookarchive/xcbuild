// Copyright 2013-present Facebook. All Rights Reserved.

#include <libutil/ISODate.h>
#include <libutil/UnixTime.h>

#include <cstring>
#include <sstream>
#include <iomanip>

using libutil::ISODate;

void ISODate::
Decode(std::string const &in, struct tm &out)
{
    std::memset(&out, 0, sizeof(tm));
    std::sscanf(in.c_str(), "%04d%02d%02dT%02d%02d%02dZ",
                &out.tm_year, &out.tm_mon, &out.tm_mday,
                &out.tm_hour, &out.tm_min, &out.tm_sec);

    out.tm_year -= 1900;
    out.tm_mon  -= 1;

    UnixTime::Decode(::mktime(&out), out);
}

std::string ISODate::
Encode(struct tm const &in)
{
    std::ostringstream os;

#define DEC(X) std::dec << std::setfill('0') << std::setw(X)

    os << DEC(4) << (in.tm_year + 1900)
       << DEC(2) << (in.tm_mon + 1)
       << DEC(2) << in.tm_mday
       << 'T'
       << DEC(2) << in.tm_hour
       << DEC(2) << in.tm_min
       << DEC(2) << in.tm_sec
       << 'Z';

    return os.str();
}
