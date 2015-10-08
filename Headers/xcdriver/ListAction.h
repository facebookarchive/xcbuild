// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_ListAction_h
#define __xcdriver_ListAction_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class ListAction {
private:
    ListAction();
    ~ListAction();

public:
    static int
    Run(Options const &options);
};

}

#endif // !__xcdriver_ListAction_h
