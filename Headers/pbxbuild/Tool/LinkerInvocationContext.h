// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_LinkerInvocationContext_h
#define __pbxbuild_LinkerInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class TypeResolvedFile;

namespace Tool {

class LinkerInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit LinkerInvocationContext(ToolInvocation const &invocation);
    ~LinkerInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static LinkerInvocationContext
    Create(
        pbxspec::PBX::Linker::shared_ptr const &linker,
        std::vector<std::string> const &inputFiles,
        std::vector<TypeResolvedFile> const &inputLibraries,
        std::string const &output,
        std::vector<std::string> const &additionalArguments,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &executable = ""
    );
};

}
}

#endif // !__pbxbuild_LinkerInvocationContext_h
