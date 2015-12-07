/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_LinkerResolver_h
#define __pbxbuild_LinkerResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>

namespace pbxbuild {

class TypeResolvedFile;

namespace Tool {

class LinkerResolver {
private:
    pbxspec::PBX::Linker::shared_ptr _linker;

public:
    explicit LinkerResolver(pbxspec::PBX::Linker::shared_ptr const &tool);
    ~LinkerResolver();

public:
    ToolInvocation invocation(
        std::vector<std::string> const &inputFiles,
        std::vector<TypeResolvedFile> const &inputLibraries,
        std::string const &output,
        std::vector<std::string> const &additionalArguments,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &executable = ""
    );

public:
    static std::string LinkerToolIdentifier()
    { return "com.apple.xcode.linkers.ld"; }
    static std::string LibtoolToolIdentifier()
    { return "com.apple.pbx.linkers.libtool"; }
    static std::string LipoToolIdentifier()
    { return "com.apple.xcode.linkers.lipo"; }

public:
    static std::unique_ptr<LinkerResolver>
    Create(Phase::PhaseEnvironment const &phaseEnvironment, std::string const &identifier);
};

}
}

#endif // !__pbxbuild_LinkerResolver_h
