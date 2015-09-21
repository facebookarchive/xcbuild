// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_CopyFilesResolver_h
#define __pbxbuild_Phase_CopyFilesResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class CopyFilesResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit CopyFilesResolver(std::vector<ToolInvocation> const &invocations);
    ~CopyFilesResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<CopyFilesResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::CopyFilesBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_CopyFilesResolver_h
