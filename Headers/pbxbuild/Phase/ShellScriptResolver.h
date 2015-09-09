// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_ShellScriptResolver_h
#define __pbxbuild_Phase_ShellScriptResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class ShellScriptResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit ShellScriptResolver(std::vector<ToolInvocation> const &invocations);
    ~ShellScriptResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<ShellScriptResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_ShellScriptResolver_h
