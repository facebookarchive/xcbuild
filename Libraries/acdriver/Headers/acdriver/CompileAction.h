/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __acdriver_CompileAction_h
#define __acdriver_CompileAction_h

namespace acdriver {

class Options;
class Output;
class Result;

/*
 * Compiles an asset catalog.
 */
class CompileAction {
public:
    CompileAction();
    ~CompileAction();

public:
    void run(Options const &options, Output *output, Result *result);
};

}

#endif // !__acdriver_CompileAction_h
