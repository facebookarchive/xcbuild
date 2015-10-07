// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_ShowSDKsAction_h
#define __xcdriver_ShowSDKsAction_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class ShowSDKsAction {
private:
    ShowSDKsAction();
    ~ShowSDKsAction();

public:
    static int
    Run(Options const &options);
};

}

#endif // !__xcdriver_ShowSDKsAction_h
