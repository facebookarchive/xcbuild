// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_VersionAction_h
#define __xcdriver_VersionAction_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class VersionAction {
private:
    VersionAction();
    ~VersionAction();

public:
    static int
    Run(Options const &options);
};

}

#endif // !__xcdriver_VersionAction_h
