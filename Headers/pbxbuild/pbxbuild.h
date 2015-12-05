/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_pbxbuild_h
#define __pbxbuild_pbxbuild_h

#include <pbxbuild/HeaderMap.h>

#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/DependencyResolver.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/WorkspaceContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetBuildRules.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/ToolInvocation.h>

#include <pbxbuild/Build/Executor.h>
#include <pbxbuild/Build/SimpleExecutor.h>
#include <pbxbuild/Build/Formatter.h>
#include <pbxbuild/Build/DefaultFormatter.h>

#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/CompilerInvocationContext.h>
#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/Tool/LinkerInvocationContext.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>

#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseInvocations.h>
#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/ShellScriptResolver.h>

#endif  // !__pbxbuild_pbxbuild_h
