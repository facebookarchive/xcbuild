// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_Driver_h
#define __xcdriver_Driver_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class Driver {
private:
    Driver();
    ~Driver();

public:
    static int
    Run(std::vector<std::string> const &args);
};

}

#endif // !__xcdriver_Driver_h
