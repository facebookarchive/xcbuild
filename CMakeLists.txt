# Copyright 2013-present Facebook. All Rights Reserved.

project(pbxbuild C CXX)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")

include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/libjson)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/expat/lib)
include_directories(${CMAKE_BINARY_DIR}/ThirdParty/expat)

add_subdirectory(ThirdParty/libjson)
add_subdirectory(ThirdParty/expat)

include_directories(Headers)
include_directories(PrivateHeaders)

set(libutil_SOURCES
    Sources/libutil/FSUtil.cpp
    Sources/libutil/SysUtil.cpp
    Sources/libutil/Crypto.cpp
    #
    Sources/libutil/Subprocess.cpp
    #
    Sources/libutil/Base64.cpp
    Sources/libutil/ISODate.cpp
    Sources/libutil/Wildcard.cpp
    Sources/libutil/UnixTime.cpp
    #
    Sources/libutil/bsd_fnmatch.c
    Sources/libutil/bsd_glob.c
    Sources/libutil/rfc4648.c
    #
    Sources/libutil/md5.c
    )

set(libplist_SOURCES
    Sources/plist/Objects.cpp
    Sources/plist/BaseXMLParser.cpp
    Sources/plist/XMLParser.cpp
    Sources/plist/SimpleXMLParser.cpp
    #
    Sources/plist/ABPCommon.cpp
    Sources/plist/ABPReader.cpp
    Sources/plist/unicode.c
    #
    Sources/plist/BinaryParser.cpp
    )

set(libpbxbase_SOURCES
    ${libutil_SOURCES}
    ${libplist_SOURCES}
    )

add_library(pbxbase SHARED ${libpbxbase_SOURCES})
target_link_libraries(pbxbase json expat)

set(libpbxsetting_SOURCES
    Sources/pbxsetting/ConfigFile.cpp
    Sources/pbxsetting/Condition.cpp
    Sources/pbxsetting/ConfigFile.cpp
    Sources/pbxsetting/DefaultSettings.cpp
    Sources/pbxsetting/Environment.cpp
    Sources/pbxsetting/Level.cpp
    Sources/pbxsetting/Setting.cpp
    Sources/pbxsetting/Type.cpp
    Sources/pbxsetting/Value.cpp
    Sources/pbxsetting/XC/Config.cpp
    )

set(libxcscheme_SOURCES
    Sources/xcscheme/SchemeGroup.cpp
    Sources/xcscheme/XC/Action.cpp
    Sources/xcscheme/XC/ActionContent.cpp
    Sources/xcscheme/XC/AdditionalOption.cpp
    Sources/xcscheme/XC/AnalyzeAction.cpp
    Sources/xcscheme/XC/ArchiveAction.cpp
    Sources/xcscheme/XC/CommandLineArgument.cpp
    Sources/xcscheme/XC/BuildAction.cpp
    Sources/xcscheme/XC/BuildActionEntry.cpp
    Sources/xcscheme/XC/BuildableReference.cpp
    Sources/xcscheme/XC/ExecuteAction.cpp
    Sources/xcscheme/XC/LaunchAction.cpp
    Sources/xcscheme/XC/LocationScenarioReference.cpp
    Sources/xcscheme/XC/ProfileAction.cpp
    Sources/xcscheme/XC/Scheme.cpp
    Sources/xcscheme/XC/Test.cpp
    Sources/xcscheme/XC/TestAction.cpp
    Sources/xcscheme/XC/TestableReference.cpp
    )

set(libpbxproj_SOURCES
    Sources/pbxproj/Context.cpp
    Sources/pbxproj/ISA.cpp
    Sources/pbxproj/PlistHelpers.cpp
    Sources/pbxproj/PBX/AggregateTarget.cpp
    Sources/pbxproj/PBX/AppleScriptBuildPhase.cpp
    Sources/pbxproj/PBX/BaseGroup.cpp
    Sources/pbxproj/PBX/BuildFile.cpp
    Sources/pbxproj/PBX/BuildPhase.cpp
    Sources/pbxproj/PBX/BuildRule.cpp
    Sources/pbxproj/PBX/CopyFilesBuildPhase.cpp
    Sources/pbxproj/PBX/FileReference.cpp
    Sources/pbxproj/PBX/FrameworksBuildPhase.cpp
    Sources/pbxproj/PBX/Group.cpp
    Sources/pbxproj/PBX/GroupItem.cpp
    Sources/pbxproj/PBX/HeadersBuildPhase.cpp
    Sources/pbxproj/PBX/LegacyTarget.cpp
    Sources/pbxproj/PBX/NativeTarget.cpp
    Sources/pbxproj/PBX/Object.cpp
    Sources/pbxproj/PBX/Project.cpp
    Sources/pbxproj/PBX/ReferenceProxy.cpp
    Sources/pbxproj/PBX/ResourcesBuildPhase.cpp
    Sources/pbxproj/PBX/ShellScriptBuildPhase.cpp
    Sources/pbxproj/PBX/SourcesBuildPhase.cpp
    Sources/pbxproj/PBX/Target.cpp
    Sources/pbxproj/PBX/VariantGroup.cpp
    Sources/pbxproj/PBX/ContainerItemProxy.cpp
    Sources/pbxproj/PBX/TargetDependency.cpp
    Sources/pbxproj/XC/BuildConfiguration.cpp
    Sources/pbxproj/XC/ConfigurationList.cpp
    Sources/pbxproj/XC/VersionGroup.cpp
    )

set(libxcworkspace_SOURCES
    Sources/xcworkspace/XC/Workspace.cpp
    Sources/xcworkspace/XC/FileRef.cpp
    Sources/xcworkspace/XC/Group.cpp
    Sources/xcworkspace/XC/GroupItem.cpp
    )

set(libxcsdk_SOURCES
    Sources/xcsdk/Environment.cpp
    Sources/xcsdk/SDK/Manager.cpp
    Sources/xcsdk/SDK/Platform.cpp
    Sources/xcsdk/SDK/PlatformVersion.cpp
    Sources/xcsdk/SDK/Product.cpp
    Sources/xcsdk/SDK/Target.cpp
    Sources/xcsdk/SDK/Toolchain.cpp
    )

set(libpbxspec_SOURCES
    Sources/pbxspec/Manager.cpp
    Sources/pbxspec/Types.cpp
    Sources/pbxspec/PBX/Architecture.cpp
    Sources/pbxspec/PBX/BuildPhase.cpp
    Sources/pbxspec/PBX/BuildRule.cpp
    Sources/pbxspec/PBX/BuildSettings.cpp
    Sources/pbxspec/PBX/BuildStep.cpp
    Sources/pbxspec/PBX/BuildSystem.cpp
    Sources/pbxspec/PBX/Compiler.cpp
    Sources/pbxspec/PBX/FileType.cpp
    Sources/pbxspec/PBX/Linker.cpp
    Sources/pbxspec/PBX/PackageType.cpp
    Sources/pbxspec/PBX/ProductType.cpp
    Sources/pbxspec/PBX/PropertyConditionFlavor.cpp
    Sources/pbxspec/PBX/PropertyOption.cpp
    Sources/pbxspec/PBX/Specification.cpp
    Sources/pbxspec/PBX/Tool.cpp
    )

set(libpbxbuild_SOURCES
    Sources/pbxbuild/BuildContext.cpp
    Sources/pbxbuild/BuildEnvironment.cpp
    Sources/pbxbuild/BuildGraph.cpp
    Sources/pbxbuild/DependencyResolver.cpp
    Sources/pbxbuild/HeaderMap.cpp
    Sources/pbxbuild/ToolInvocation.cpp
    Sources/pbxbuild/Tool/SearchPaths.cpp
    Sources/pbxbuild/Tool/CopyInvocationContext.cpp
    Sources/pbxbuild/Tool/HeadermapInvocationContext.cpp
    Sources/pbxbuild/Tool/CompilerInvocationContext.cpp
    Sources/pbxbuild/Tool/LinkerInvocationContext.cpp
    Sources/pbxbuild/Tool/ScriptInvocationContext.cpp
    Sources/pbxbuild/Tool/ToolInvocationContext.cpp
    Sources/pbxbuild/TargetBuildRules.cpp
    Sources/pbxbuild/TargetEnvironment.cpp
    Sources/pbxbuild/TypeResolvedFile.cpp
    Sources/pbxbuild/Phase/PhaseContext.cpp
    Sources/pbxbuild/Phase/PhaseInvocations.cpp
    Sources/pbxbuild/Phase/CopyFilesResolver.cpp
    Sources/pbxbuild/Phase/HeadersResolver.cpp
    Sources/pbxbuild/Phase/ResourcesResolver.cpp
    Sources/pbxbuild/Phase/FrameworksResolver.cpp
    Sources/pbxbuild/Phase/SourcesResolver.cpp
    Sources/pbxbuild/Phase/ShellScriptResolver.cpp
    )

set(libxcdriver_SOURCES
    Sources/xcdriver/Action.cpp
    Sources/xcdriver/Driver.cpp
    Sources/xcdriver/Options.cpp)

add_library(pbxsetting SHARED ${libpbxsetting_SOURCES})
target_link_libraries(pbxsetting pbxbase)

add_library(xcsdk SHARED ${libxcsdk_SOURCES})
target_link_libraries(xcsdk pbxsetting pbxbase)

add_library(xcscheme SHARED ${libxcscheme_SOURCES})
target_link_libraries(xcscheme pbxbase)

add_library(pbxproj SHARED ${libpbxproj_SOURCES})
target_link_libraries(pbxproj pbxsetting pbxbase)

add_library(xcworkspace SHARED ${libxcworkspace_SOURCES})
target_link_libraries(xcworkspace pbxbase)

add_library(pbxspec SHARED ${libpbxspec_SOURCES})
target_link_libraries(pbxspec pbxsetting pbxbase)

add_library(pbxbuild SHARED ${libpbxbuild_SOURCES})
target_link_libraries(pbxbuild xcsdk xcworkspace xcscheme pbxproj pbxspec)

add_library(xcdriver SHARED ${libxcdriver_SOURCES})
target_link_libraries(xcdriver pbxsetting pbxbase)

add_executable(plist Tools/plist.cpp)
target_link_libraries(plist pbxbase)
add_executable(sdktool Tools/sdktool.cpp)
target_link_libraries(sdktool xcsdk)
add_executable(xcconfig Tools/xcconfig.cpp)
target_link_libraries(xcconfig pbxsetting)
add_executable(hmap Tools/hmap.cpp)
target_link_libraries(hmap pbxbase pbxbuild)
add_executable(pbxtool Tools/pbxtool.cpp)
target_link_libraries(pbxtool pbxbase pbxproj xcscheme pbxsetting)
add_executable(xcspec Tools/xcspec.cpp)
target_link_libraries(xcspec pbxspec)
add_executable(xctool Tools/xctool.cpp)
target_link_libraries(xctool pbxbase pbxproj xcscheme xcworkspace)
add_executable(settings Tools/settings.cpp)
target_link_libraries(settings pbxbuild pbxbase pbxproj xcsdk pbxsetting pbxspec)
add_executable(build Tools/build.cpp)
target_link_libraries(build pbxbase pbxproj xcsdk pbxsetting pbxspec xcworkspace xcscheme pbxbuild)
add_executable(xcbuild Tools/xcbuild.cpp)
target_link_libraries(xcbuild xcdriver)

enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/gtest/include)
add_subdirectory(ThirdParty/gtest)

function (ADD_UNIT_GTEST NAME SOURCES)
    add_executable("${NAME}" ${SOURCES})
    target_link_libraries("${NAME}" gtest gtest_main)
    add_test("${NAME}" "${NAME}")
endfunction ()

ADD_UNIT_GTEST(test_FSUtil Tests/libutil/test_FSUtil.cpp)
target_link_libraries(test_FSUtil pbxbase)

ADD_UNIT_GTEST(test_Wildcard Tests/libutil/test_Wildcard.cpp)
target_link_libraries(test_Wildcard pbxbase)

ADD_UNIT_GTEST(test_pbxsetting Tests/pbxsetting/test_pbxsetting.cpp)
target_link_libraries(test_pbxsetting pbxsetting)
