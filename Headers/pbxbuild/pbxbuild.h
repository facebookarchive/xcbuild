// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_pbxbuild_h
#define __pbxbuild_pbxbuild_h

#include <pbxbuild/HeaderMap.h>

#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/DependencyResolver.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetBuildRules.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/ToolInvocation.h>

#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/CompilerInvocationContext.h>
#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/Tool/LinkerInvocationContext.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>

#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/ShellScriptResolver.h>

#endif  // !__pbxbuild_pbxbuild_h
