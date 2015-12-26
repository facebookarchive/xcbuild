

/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_PhaseContext_h
#define __pbxbuild_Phase_PhaseContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Tool/ToolContext.h>

namespace pbxbuild {

class TypeResolvedFile;

namespace Tool {
    class ClangResolver;
    class CopyResolver;
    class InfoPlistResolver;
    class ScriptResolver;
    class TouchResolver;
    class ToolResolver;
}

namespace Phase {

class PhaseEnvironment;

class PhaseContext {
private:
    Tool::ToolContext                                   _toolContext;

private:
    std::unique_ptr<Tool::ClangResolver>                _clangResolver;
    std::unique_ptr<Tool::CopyResolver>                 _copyResolver;
    std::unique_ptr<Tool::InfoPlistResolver>            _infoPlistResolver;
    std::unique_ptr<Tool::ScriptResolver>               _scriptResolver;
    std::unique_ptr<Tool::TouchResolver>                _touchResolver;
    std::unordered_map<std::string, Tool::ToolResolver> _toolResolvers;

public:
    explicit PhaseContext(Tool::ToolContext const &toolContext);
    ~PhaseContext();

public:
    Tool::ToolContext const &toolContext() const
    { return _toolContext; }

public:
    Tool::ToolContext &toolContext()
    { return _toolContext; }

public:
    Tool::ClangResolver const     *clangResolver(Phase::PhaseEnvironment const &phaseEnvironment);
    Tool::CopyResolver const      *copyResolver(Phase::PhaseEnvironment const &phaseEnvironment);
    Tool::InfoPlistResolver const *infoPlistResolver(Phase::PhaseEnvironment const &phaseEnvironment);
    Tool::ScriptResolver const    *scriptResolver(Phase::PhaseEnvironment const &phaseEnvironment);
    Tool::TouchResolver const     *touchResolver(Phase::PhaseEnvironment const &phaseEnvironment);
    Tool::ToolResolver const      *toolResolver(Phase::PhaseEnvironment const &phaseEnvironment, std::string const &identifier);

public:
    bool resolveBuildFile(
        Phase::PhaseEnvironment const &phaseEnvironment,
        pbxsetting::Environment const &environment,
        pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase,
        pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
        TypeResolvedFile const &file,
        std::string const &outputDirectory,
        std::string const &fallbackToolIdentifier = std::string());
};

}
}

#endif // !__pbxbuild_Phase_PhaseContext_h
