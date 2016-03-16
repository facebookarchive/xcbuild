/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __acdriver_ContentsAction_h
#define __acdriver_ContentsAction_h

namespace acdriver {

class Options;
class Output;
class Result;

/*
 * Prints the contents of an asset catalog.
 */
class ContentsAction {
public:
    ContentsAction();
    ~ContentsAction();

public:
    void run(Options const &options, Output *output, Result *result);
};

}

#endif // !__acdriver_ContentsAction_h
