#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

project(pbxbuild C CXX)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")

include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/expat/expat/lib)
include_directories(${CMAKE_BINARY_DIR}/ThirdParty/expat/expat)

add_subdirectory(ThirdParty/expat/expat)

find_library(CORE_FOUNDATION CoreFoundation)
find_library(CORE_SERVICES CoreSERVICES)

include_directories(Headers)
include_directories(PrivateHeaders)

set(libutil_SOURCES
    Sources/libutil/FSUtil.cpp
    Sources/libutil/SysUtil.cpp
    Sources/libutil/Crypto.cpp
    Sources/libutil/Options.cpp
    #
    Sources/libutil/Subprocess.cpp
    #
    Sources/libutil/Base64.cpp
    Sources/libutil/ISODate.cpp
    Sources/libutil/Wildcard.cpp
    Sources/libutil/UnixTime.cpp
    #
    Sources/libutil/bsd_glob.c
    Sources/libutil/rfc4648.c
    #
    Sources/libutil/md5.c
    )

set(libplist_SOURCES
    Sources/plist/Array.cpp
    Sources/plist/Boolean.cpp
    Sources/plist/Data.cpp
    Sources/plist/Date.cpp
    Sources/plist/Dictionary.cpp
    Sources/plist/Integer.cpp
    Sources/plist/Null.cpp
    Sources/plist/Object.cpp
    Sources/plist/Real.cpp
    Sources/plist/String.cpp
    Sources/plist/Keys.cpp
    #
    Sources/plist/Format/Encoding.cpp
    Sources/plist/Format/unicode.c
    #
    Sources/plist/Format/BaseXMLParser.cpp
    Sources/plist/Format/XMLParser.cpp
    Sources/plist/Format/XMLWriter.cpp
    Sources/plist/Format/XML.cpp
    #
    Sources/plist/Format/SimpleXMLParser.cpp
    Sources/plist/Format/SimpleXML.cpp
    #
    Sources/plist/Format/ABPCommon.cpp
    Sources/plist/Format/ABPReader.cpp
    Sources/plist/Format/ABPWriter.cpp
    Sources/plist/Format/Binary.cpp
    #
    Sources/plist/Format/ASCIIPListLexer.cpp
    Sources/plist/Format/Strings.cpp
    Sources/plist/Format/ASCIIParser.cpp
    Sources/plist/Format/ASCIIWriter.cpp
    Sources/plist/Format/ASCII.cpp
    #
    Sources/plist/Format/Any.cpp
    )

set(libbuiltin_SOURCES
    Sources/builtin/Driver.cpp
    Sources/builtin/Registry.cpp
    #
    Sources/builtin/copy/Options.cpp
    Sources/builtin/copy/Driver.cpp
    #
    Sources/builtin/copyPlist/Options.cpp
    Sources/builtin/copyPlist/Driver.cpp
    #
    Sources/builtin/copyStrings/Options.cpp
    Sources/builtin/copyStrings/Driver.cpp
    #
    Sources/builtin/copyTiff/Options.cpp
    Sources/builtin/copyTiff/Driver.cpp
    #
    Sources/builtin/infoPlistUtility/Options.cpp
    Sources/builtin/infoPlistUtility/Driver.cpp
    #
    Sources/builtin/lsRegisterURL/Options.cpp
    Sources/builtin/lsRegisterURL/Driver.cpp
    #
    Sources/builtin/productPackagingUtility/Options.cpp
    Sources/builtin/productPackagingUtility/Driver.cpp
    #
    Sources/builtin/validationUtility/Options.cpp
    Sources/builtin/validationUtility/Driver.cpp
    #
    Sources/builtin/embeddedBinaryValidationUtility/Options.cpp
    Sources/builtin/embeddedBinaryValidationUtility/Driver.cpp
    )

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
    Sources/pbxbuild/WorkspaceContext.cpp
    Sources/pbxbuild/TargetBuildRules.cpp
    Sources/pbxbuild/TargetEnvironment.cpp
    Sources/pbxbuild/TypeResolvedFile.cpp
    Sources/pbxbuild/Build/Executor.cpp
    Sources/pbxbuild/Build/SimpleExecutor.cpp
    Sources/pbxbuild/Build/Formatter.cpp
    Sources/pbxbuild/Build/DefaultFormatter.cpp
    Sources/pbxbuild/ToolInvocation.cpp
    Sources/pbxbuild/Tool/CommandLineResult.cpp
    Sources/pbxbuild/Tool/OptionsResult.cpp
    Sources/pbxbuild/Tool/ToolEnvironment.cpp
    Sources/pbxbuild/Tool/PrecompiledHeaderInfo.cpp
    Sources/pbxbuild/Tool/SearchPaths.cpp
    Sources/pbxbuild/Tool/CopyInvocationContext.cpp
    Sources/pbxbuild/Tool/HeadermapInvocationContext.cpp
    Sources/pbxbuild/Tool/CompilerInvocationContext.cpp
    Sources/pbxbuild/Tool/LinkerInvocationContext.cpp
    Sources/pbxbuild/Tool/ScriptInvocationContext.cpp
    Sources/pbxbuild/Tool/ToolInvocationContext.cpp
    Sources/pbxbuild/Phase/PhaseContext.cpp
    Sources/pbxbuild/Phase/PhaseInvocations.cpp
    Sources/pbxbuild/Phase/CopyFilesResolver.cpp
    Sources/pbxbuild/Phase/HeadersResolver.cpp
    Sources/pbxbuild/Phase/LegacyTargetResolver.cpp
    Sources/pbxbuild/Phase/ResourcesResolver.cpp
    Sources/pbxbuild/Phase/FrameworksResolver.cpp
    Sources/pbxbuild/Phase/SourcesResolver.cpp
    Sources/pbxbuild/Phase/ShellScriptResolver.cpp
    )

set(libxcdriver_SOURCES
    Sources/xcdriver/Action.cpp
    Sources/xcdriver/Driver.cpp
    Sources/xcdriver/Options.cpp
    Sources/xcdriver/BuildAction.cpp
    Sources/xcdriver/FindAction.cpp
    Sources/xcdriver/ListAction.cpp
    Sources/xcdriver/ShowBuildSettingsAction.cpp
    Sources/xcdriver/ShowSDKsAction.cpp
    Sources/xcdriver/VersionAction.cpp)

add_library(util SHARED ${libutil_SOURCES})
target_link_libraries(util)

add_library(plist SHARED ${libplist_SOURCES})
target_link_libraries(plist util expat)

add_library(builtin SHARED ${libbuiltin_SOURCES})
target_link_libraries(builtin util plist ${CORE_FOUNDATION} ${CORE_SERVICES})

add_library(pbxsetting SHARED ${libpbxsetting_SOURCES})
target_link_libraries(pbxsetting util plist)

add_library(xcsdk SHARED ${libxcsdk_SOURCES})
target_link_libraries(xcsdk pbxsetting util plist)

add_library(xcscheme SHARED ${libxcscheme_SOURCES})
target_link_libraries(xcscheme util plist)

add_library(pbxproj SHARED ${libpbxproj_SOURCES})
target_link_libraries(pbxproj pbxsetting util plist)

add_library(xcworkspace SHARED ${libxcworkspace_SOURCES})
target_link_libraries(xcworkspace util plist)

add_library(pbxspec SHARED ${libpbxspec_SOURCES})
target_link_libraries(pbxspec pbxsetting util plist)

add_library(pbxbuild SHARED ${libpbxbuild_SOURCES})
target_link_libraries(pbxbuild xcsdk xcworkspace xcscheme pbxproj pbxspec builtin)

add_library(xcdriver SHARED ${libxcdriver_SOURCES})
target_link_libraries(xcdriver pbxbuild xcworkspace xcsdk pbxsetting util plist builtin)

add_executable(plutil Tools/plutil.cpp)
target_link_libraries(plutil plist)
add_executable(xcconfig Tools/xcconfig.cpp)
target_link_libraries(xcconfig pbxsetting)
add_executable(hmap Tools/hmap.cpp)
target_link_libraries(hmap util plist pbxbuild)
add_executable(pbxtool Tools/pbxtool.cpp)
target_link_libraries(pbxtool util plist pbxproj xcscheme pbxsetting)
add_executable(xcspec Tools/xcspec.cpp)
target_link_libraries(xcspec pbxspec)
add_executable(xctool Tools/xctool.cpp)
target_link_libraries(xctool util plist pbxproj xcscheme xcworkspace)
add_executable(xcbuild Tools/xcbuild.cpp)
target_link_libraries(xcbuild xcdriver)

add_executable(builtin-copy Tools/builtin/copy.cpp)
target_link_libraries(builtin-copy builtin)
add_executable(builtin-copyPlist Tools/builtin/copyPlist.cpp)
target_link_libraries(builtin-copyPlist builtin)
add_executable(builtin-copyStrings Tools/builtin/copyStrings.cpp)
target_link_libraries(builtin-copyStrings builtin)
add_executable(builtin-copyTiff Tools/builtin/copyTiff.cpp)
target_link_libraries(builtin-copyTiff builtin)
add_executable(builtin-infoPlistUtility Tools/builtin/infoPlistUtility.cpp)
target_link_libraries(builtin-infoPlistUtility builtin)
add_executable(builtin-lsRegisterURL Tools/builtin/lsRegisterURL.cpp)
target_link_libraries(builtin-lsRegisterURL builtin)
add_executable(builtin-productPackagingUtility Tools/builtin/productPackagingUtility.cpp)
target_link_libraries(builtin-productPackagingUtility builtin)
add_executable(builtin-validationUtility Tools/builtin/validationUtility.cpp)
target_link_libraries(builtin-validationUtility builtin)
add_executable(builtin-embeddedBinaryValidationUtility Tools/builtin/embeddedBinaryValidationUtility.cpp)
target_link_libraries(builtin-embeddedBinaryValidationUtility builtin)

enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest/include)
add_subdirectory(ThirdParty/googletest/googletest)

function (ADD_UNIT_GTEST NAME SOURCES)
    add_executable("${NAME}" ${SOURCES})
    target_link_libraries("${NAME}" gtest gtest_main)
    add_test("${NAME}" "${NAME}")
endfunction ()

ADD_UNIT_GTEST(test_FSUtil Tests/libutil/test_FSUtil.cpp)
target_link_libraries(test_FSUtil util)

ADD_UNIT_GTEST(test_Wildcard Tests/libutil/test_Wildcard.cpp)
target_link_libraries(test_Wildcard util)

ADD_UNIT_GTEST(test_Condition Tests/pbxsetting/test_Condition.cpp)
target_link_libraries(test_Condition pbxsetting)

ADD_UNIT_GTEST(test_Environment Tests/pbxsetting/test_Environment.cpp)
target_link_libraries(test_Environment pbxsetting)

ADD_UNIT_GTEST(test_Setting Tests/pbxsetting/test_Setting.cpp)
target_link_libraries(test_Setting pbxsetting)

ADD_UNIT_GTEST(test_Type Tests/pbxsetting/test_Type.cpp)
target_link_libraries(test_Type pbxsetting)

ADD_UNIT_GTEST(test_Value Tests/pbxsetting/test_Value.cpp)
target_link_libraries(test_Value pbxsetting)
