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
set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif ()
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

find_library(CORE_FOUNDATION CoreFoundation)
if ("${CORE_FOUNDATION}" STREQUAL "CORE_FOUNDATION-NOTFOUND")
  set(CORE_FOUNDATION "")
endif ()

find_library(CORE_SERVICES CoreServices)
if ("${CORE_SERVICES}" STREQUAL "CORE_SERVICES-NOTFOUND")
  set(CORE_SERVICES "")
endif ()

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

include_directories(Headers)
include_directories(PrivateHeaders)

set(libext_SOURCES
    Sources/ext/optional.cpp)

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
    Sources/plist/UID.cpp
    #
    Sources/plist/Base64.cpp
    Sources/plist/rfc4648.c
    Sources/plist/ISODate.cpp
    Sources/plist/UnixTime.cpp
    #
    Sources/plist/Keys/Unpack.cpp
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
    Sources/plist/Format/ASCIIParser.cpp
    Sources/plist/Format/ASCIIWriter.cpp
    Sources/plist/Format/ASCII.cpp
    #
    Sources/plist/Format/JSONWriter.cpp
    Sources/plist/Format/JSON.cpp
    #
    Sources/plist/Format/Any.cpp
    )

set(libninja_SOURCES
    Sources/ninja/Writer.cpp
    Sources/ninja/Value.cpp
    )

set(libutil_SOURCES
    Sources/libutil/FSUtil.cpp
    Sources/libutil/SysUtil.cpp
    Sources/libutil/Options.cpp
    #
    Sources/libutil/Subprocess.cpp
    #
    Sources/libutil/Escape.cpp
    Sources/libutil/Wildcard.cpp
    #
    Sources/libutil/bsd_glob.c
    #
    Sources/libutil/md5.c
    )

set(libdependency_SOURCES
    Sources/dependency/DependencyInfo.cpp
    Sources/dependency/DependencyInfoFormat.cpp
    Sources/dependency/BinaryDependencyInfo.cpp
    Sources/dependency/DirectoryDependencyInfo.cpp
    Sources/dependency/MakefileDependencyInfo.cpp
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
    Sources/pbxbuild/DirectedGraph.cpp
    Sources/pbxbuild/HeaderMap.cpp
    Sources/pbxbuild/WorkspaceContext.cpp
    Sources/pbxbuild/FileTypeResolver.cpp
    Sources/pbxbuild/Tool/Context.cpp
    Sources/pbxbuild/Tool/Environment.cpp
    Sources/pbxbuild/Tool/Invocation.cpp
    Sources/pbxbuild/Tool/Tokens.cpp
    Sources/pbxbuild/Tool/OptionsResult.cpp
    Sources/pbxbuild/Tool/CompilationInfo.cpp
    Sources/pbxbuild/Tool/SwiftModuleInfo.cpp
    Sources/pbxbuild/Tool/HeadermapInfo.cpp
    Sources/pbxbuild/Tool/PrecompiledHeaderInfo.cpp
    Sources/pbxbuild/Tool/SearchPaths.cpp
    Sources/pbxbuild/Tool/CopyResolver.cpp
    Sources/pbxbuild/Tool/DittoResolver.cpp
    Sources/pbxbuild/Tool/SymlinkResolver.cpp
    Sources/pbxbuild/Tool/MakeDirectoryResolver.cpp
    Sources/pbxbuild/Tool/HeadermapResolver.cpp
    Sources/pbxbuild/Tool/InfoPlistResolver.cpp
    Sources/pbxbuild/Tool/AssetCatalogResolver.cpp
    Sources/pbxbuild/Tool/InterfaceBuilderResolver.cpp
    Sources/pbxbuild/Tool/InterfaceBuilderStoryboardLinkerResolver.cpp
    Sources/pbxbuild/Tool/InterfaceBuilderCommon.cpp
    Sources/pbxbuild/Tool/ClangResolver.cpp
    Sources/pbxbuild/Tool/CompilerCommon.cpp
    Sources/pbxbuild/Tool/LinkerResolver.cpp
    Sources/pbxbuild/Tool/ScriptResolver.cpp
    Sources/pbxbuild/Tool/SwiftResolver.cpp
    Sources/pbxbuild/Tool/SwiftStandardLibraryResolver.cpp
    Sources/pbxbuild/Tool/TouchResolver.cpp
    Sources/pbxbuild/Tool/ToolResolver.cpp
    Sources/pbxbuild/Phase/Context.cpp
    Sources/pbxbuild/Phase/Environment.cpp
    Sources/pbxbuild/Phase/File.cpp
    Sources/pbxbuild/Phase/PhaseInvocations.cpp
    Sources/pbxbuild/Phase/CopyFilesResolver.cpp
    Sources/pbxbuild/Phase/HeadersResolver.cpp
    Sources/pbxbuild/Phase/LegacyTargetResolver.cpp
    Sources/pbxbuild/Phase/ProductTypeResolver.cpp
    Sources/pbxbuild/Phase/SwiftResolver.cpp
    Sources/pbxbuild/Phase/ResourcesResolver.cpp
    Sources/pbxbuild/Phase/FrameworksResolver.cpp
    Sources/pbxbuild/Phase/SourcesResolver.cpp
    Sources/pbxbuild/Phase/ShellScriptResolver.cpp
    Sources/pbxbuild/Target/BuildRules.cpp
    Sources/pbxbuild/Target/Environment.cpp
    Sources/pbxbuild/Build/Context.cpp
    Sources/pbxbuild/Build/Environment.cpp
    Sources/pbxbuild/Build/DependencyResolver.cpp
    )

set(libxcexecution_SOURCES
    Sources/xcexecution/Executor.cpp
    Sources/xcexecution/SimpleExecutor.cpp
    Sources/xcexecution/NinjaExecutor.cpp
    Sources/xcexecution/Formatter.cpp
    Sources/xcexecution/DefaultFormatter.cpp
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
    Sources/xcdriver/VersionAction.cpp
    )

add_library(ext SHARED ${libext_SOURCES})
target_link_libraries(ext)

add_library(ninja SHARED ${libninja_SOURCES})
target_link_libraries(ninja)

add_library(plist SHARED ${libplist_SOURCES})
target_link_libraries(plist xml2)

add_library(util SHARED ${libutil_SOURCES})
target_link_libraries(util)

add_library(dependency SHARED ${libdependency_SOURCES})
target_link_libraries(dependency util)

add_library(pbxsetting SHARED ${libpbxsetting_SOURCES})
target_link_libraries(pbxsetting util plist)

add_library(builtin SHARED ${libbuiltin_SOURCES})
target_link_libraries(builtin util plist pbxsetting ${CORE_FOUNDATION} ${CORE_SERVICES})

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
target_link_libraries(pbxbuild xcsdk xcworkspace xcscheme pbxproj pbxspec dependency ext)

add_library(xcexecution SHARED ${libxcexecution_SOURCES})
target_link_libraries(xcexecution pbxbuild dependency ninja builtin)

add_library(xcdriver SHARED ${libxcdriver_SOURCES})
target_link_libraries(xcdriver xcexecution pbxbuild xcworkspace xcsdk pbxsetting util plist builtin)

add_executable(plutil Tools/plutil.cpp)
target_link_libraries(plutil plist util)
add_executable(xcrun Tools/xcrun.cpp)
target_link_libraries(xcrun xcsdk util)
add_executable(xcbuild Tools/xcbuild.cpp)
target_link_libraries(xcbuild xcdriver)
add_executable(ninja-dependency-info Tools/ninja-dependency-info.cpp)
target_link_libraries(ninja-dependency-info dependency util)

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

add_executable(dump_xcconfig Tools/dump/xcconfig.cpp)
target_link_libraries(dump_xcconfig pbxsetting)
add_executable(dump_hmap Tools/dump/hmap.cpp)
target_link_libraries(dump_hmap util plist pbxbuild)
add_executable(dump_xcodeproj Tools/dump/xcodeproj.cpp)
target_link_libraries(dump_xcodeproj util plist pbxproj xcscheme pbxsetting)
add_executable(dump_xcspec Tools/dump/xcspec.cpp)
target_link_libraries(dump_xcspec pbxspec)
add_executable(dump_xcworkspace Tools/dump/xcworkspace.cpp)
target_link_libraries(dump_xcworkspace util plist pbxproj xcscheme xcworkspace)

enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest/include)
add_subdirectory(ThirdParty/googletest/googletest)

function (ADD_UNIT_GTEST LIBRARY NAME SOURCES)
    set(TARGET_NAME "test_${LIBRARY}_${NAME}")
    add_executable("${TARGET_NAME}" ${SOURCES})
    target_link_libraries("${TARGET_NAME}" "${LIBRARY}" gtest gtest_main)
    add_test("${TARGET_NAME}" "${TARGET_NAME}")
endfunction ()

ADD_UNIT_GTEST(util FSUtil Tests/libutil/test_FSUtil.cpp)
ADD_UNIT_GTEST(util Wildcard Tests/libutil/test_Wildcard.cpp)
ADD_UNIT_GTEST(util Escape Tests/libutil/test_Escape.cpp)

ADD_UNIT_GTEST(plist Boolean Tests/plist/test_Boolean.cpp)
ADD_UNIT_GTEST(plist String Tests/plist/test_String.cpp)
ADD_UNIT_GTEST(plist Encoding Tests/plist/Format/test_Encoding.cpp)
ADD_UNIT_GTEST(plist ASCII Tests/plist/Format/test_ASCII.cpp)
ADD_UNIT_GTEST(plist JSON Tests/plist/Format/test_JSON.cpp)
ADD_UNIT_GTEST(plist XML Tests/plist/Format/test_XML.cpp)

ADD_UNIT_GTEST(ninja Value Tests/ninja/test_Value.cpp)
ADD_UNIT_GTEST(ninja Writer Tests/ninja/test_Writer.cpp)

ADD_UNIT_GTEST(dependency BinaryDependencyInfo Tests/dependency/test_BinaryDependencyInfo.cpp)
ADD_UNIT_GTEST(dependency MakefileDependencyInfo Tests/dependency/test_MakefileDependencyInfo.cpp)

ADD_UNIT_GTEST(pbxsetting Condition Tests/pbxsetting/test_Condition.cpp)
ADD_UNIT_GTEST(pbxsetting Environment Tests/pbxsetting/test_Environment.cpp)
ADD_UNIT_GTEST(pbxsetting Setting Tests/pbxsetting/test_Setting.cpp)
ADD_UNIT_GTEST(pbxsetting Type Tests/pbxsetting/test_Type.cpp)
ADD_UNIT_GTEST(pbxsetting Value Tests/pbxsetting/test_Value.cpp)

ADD_UNIT_GTEST(pbxbuild DirectedGraph Tests/pbxbuild/test_DirectedGraph.cpp)
ADD_UNIT_GTEST(pbxbuild OptionsResolver Tests/pbxbuild/test_OptionsResolver.cpp)

ADD_UNIT_GTEST(xcdriver Options Tests/xcdriver/test_Options.cpp)
ADD_UNIT_GTEST(xcdriver Action Tests/xcdriver/test_Action.cpp)
