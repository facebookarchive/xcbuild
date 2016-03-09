/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __acdriver_Driver_h
#define __acdriver_Driver_h

#include <string>
#include <vector>

namespace acdriver {

/*
 * Implements the actool command line tool.
 */
class Driver {
private:
    Driver();
    ~Driver();

public:
    static int
    Run(std::vector<std::string> const &args);
};

}

#endif // !__acdriver_Driver_h
