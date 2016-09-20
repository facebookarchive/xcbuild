/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcdriver_BuildAction_h
#define __xcdriver_BuildAction_h

namespace libutil { class Filesystem; }
namespace libutil { class ProcessContext; }

namespace xcdriver {

class Options;

class BuildAction {
private:
    BuildAction();
    ~BuildAction();

public:
    static int
    Run(libutil::ProcessContext const *processContext, libutil::Filesystem *filesystem, Options const &options);
};

}

#endif // !__xcdriver_BuildAction_h
