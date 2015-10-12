// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_BuildAction_h
#define __xcdriver_BuildAction_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class BuildAction {
private:
    BuildAction();
    ~BuildAction();

public:
    static int
    Run(Options const &options);
};

}

#endif // !__xcdriver_BuildAction_h
