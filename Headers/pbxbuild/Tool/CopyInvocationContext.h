/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_CopyInvocationContext_h
#define __pbxbuild_CopyInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Tool {

class CopyInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit CopyInvocationContext(ToolInvocation const &invocation);
    ~CopyInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static CopyInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &copyTool,
        std::string const &inputFile,
        std::string const &outputDirectory,
        std::string const &logMessageTitle,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}
}

#endif // !__pbxbuild_CopyInvocationContext_h
