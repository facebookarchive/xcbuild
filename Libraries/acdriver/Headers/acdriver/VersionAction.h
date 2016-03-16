/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __acdriver_VersionAction_h
#define __acdriver_VersionAction_h

namespace acdriver {

class Options;
class Output;
class Result;

/*
 * Prints the version of the compiler.
 */
class VersionAction {
public:
    VersionAction();
    ~VersionAction();

public:
    void run(Options const &options, Output *output, Result *result);
};

}

#endif // !__acdriver_VersionAction_h
