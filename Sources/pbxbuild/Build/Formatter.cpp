// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Build/Formatter.h>

using pbxbuild::Build::Formatter;

Formatter::
Formatter()
{
}

Formatter::
~Formatter()
{
}

void Formatter::
Print(std::string const &output)
{
    fputs(output.c_str(), stdout);
}

