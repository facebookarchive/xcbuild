// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_PhaseContext_h
#define __pbxbuild_Phase_PhaseContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/TargetEnvironment.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext {
private:
    BuildEnvironment  _buildEnvironment;
    BuildContext      _buildContext;
    TargetEnvironment _targetEnvironment;

public:
    PhaseContext(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, TargetEnvironment const &targetEnvironment);
    ~PhaseContext();

public:
    BuildEnvironment const &buildEnvironment() const
    { return _buildEnvironment; }
    BuildContext const &buildContext() const
    { return _buildContext; }
    TargetEnvironment const &targetEnvironment() const
    { return _targetEnvironment; }

public:
    std::unique_ptr<pbxbuild::TypeResolvedFile>
    resolveBuildFile(pbxsetting::Environment const &environment, pbxproj::PBX::BuildFile::shared_ptr const &buildFile) const;
    std::vector<std::pair<pbxproj::PBX::BuildFile::shared_ptr, pbxbuild::TypeResolvedFile>>
    resolveBuildFiles(pbxsetting::Environment const &environment, std::vector<pbxproj::PBX::BuildFile::shared_ptr> const &buildFiles) const;

public:
    static pbxsetting::Level
    VariantLevel(std::string const &variant);
    static pbxsetting::Level
    ArchitectureLevel(std::string const &arch);
};

}
}

#endif // !__pbxbuild_Phase_PhaseContext_h
