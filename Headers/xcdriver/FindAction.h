// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_FindAction_h
#define __xcdriver_FindAction_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class FindAction {
private:
    FindAction();
    ~FindAction();

public:
    static int
    Run(Options const &options);
};

}

#endif // !__xcdriver_FindAction_h
