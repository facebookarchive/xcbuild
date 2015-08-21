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
    Sources/libutil/UnixTime.cpp
    #
    Sources/libutil/bsd_fnmatch.c
    Sources/libutil/bsd_glob.c
    Sources/libutil/rfc4648.c
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
    Sources/pbxsetting/Value.cpp
    Sources/pbxsetting/XC/Config.cpp
    )

set(libpbxproj_SOURCES
    Sources/pbxproj/Context.cpp
    Sources/pbxproj/ISA.cpp
    Sources/pbxproj/PlistHelpers.cpp
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
    Sources/pbxproj/PBX/ResourcesBuildPhase.cpp
    Sources/pbxproj/PBX/ShellScriptBuildPhase.cpp
    Sources/pbxproj/PBX/SourcesBuildPhase.cpp
    Sources/pbxproj/PBX/Target.cpp
    Sources/pbxproj/PBX/VariantGroup.cpp
    Sources/pbxproj/PBX/ContainerItemProxy.cpp
    Sources/pbxproj/PBX/TargetDependency.cpp
    Sources/pbxproj/XC/Action.cpp
    Sources/pbxproj/XC/ActionContent.cpp
    Sources/pbxproj/XC/AdditionalOption.cpp
    Sources/pbxproj/XC/AnalyzeAction.cpp
    Sources/pbxproj/XC/ArchiveAction.cpp
    Sources/pbxproj/XC/BuildAction.cpp
    Sources/pbxproj/XC/BuildActionEntry.cpp
    Sources/pbxproj/XC/BuildConfiguration.cpp
    Sources/pbxproj/XC/BuildableReference.cpp
    Sources/pbxproj/XC/CommandLineArgument.cpp
    Sources/pbxproj/XC/ConfigurationList.cpp
    Sources/pbxproj/XC/ExecuteAction.cpp
    Sources/pbxproj/XC/LaunchAction.cpp
    Sources/pbxproj/XC/LocationScenarioReference.cpp
    Sources/pbxproj/XC/ProfileAction.cpp
    Sources/pbxproj/XC/Scheme.cpp
    Sources/pbxproj/XC/Test.cpp
    Sources/pbxproj/XC/TestAction.cpp
    Sources/pbxproj/XC/TestableReference.cpp
    Sources/pbxproj/XC/VersionGroup.cpp
    Sources/pbxproj/XC/Workspace.cpp
    Sources/pbxproj/XC/WorkspaceFileRef.cpp
    Sources/pbxproj/XC/WorkspaceGroup.cpp
    Sources/pbxproj/XC/WorkspaceGroupItem.cpp
    )

set(libxcsdk_SOURCES
    Sources/xcsdk/SDK/Manager.cpp
    Sources/xcsdk/SDK/Platform.cpp
    Sources/xcsdk/SDK/PlatformVersion.cpp
    Sources/xcsdk/SDK/Product.cpp
    Sources/xcsdk/SDK/Target.cpp
    Sources/xcsdk/SDK/Toolchain.cpp
    )

set(libpbxspec_SOURCES
    Sources/pbxspec/ISA.cpp
    Sources/pbxspec/Manager.cpp
    Sources/pbxspec/Types.cpp
    Sources/pbxspec/PBX/ApplicationProductType.cpp
    Sources/pbxspec/PBX/ApplicationWrapperFileType.cpp
    Sources/pbxspec/PBX/Architecture.cpp
    Sources/pbxspec/PBX/BuildPhase.cpp
    Sources/pbxspec/PBX/BuildSystem.cpp
    Sources/pbxspec/PBX/BundleProductType.cpp
    Sources/pbxspec/PBX/CFBundleWrapperFileType.cpp
    Sources/pbxspec/PBX/Compiler.cpp
    Sources/pbxspec/PBX/CompilerSpecificationGcc.cpp
    Sources/pbxspec/PBX/CompilerSpecificationLex.cpp
    Sources/pbxspec/PBX/CompilerSpecificationMig.cpp
    Sources/pbxspec/PBX/CompilerSpecificationOpenCL.cpp
    Sources/pbxspec/PBX/CompilerSpecificationPbxCp.cpp
    Sources/pbxspec/PBX/CompilerSpecificationRez.cpp
    Sources/pbxspec/PBX/CompilerSpecificationYacc.cpp
    Sources/pbxspec/PBX/DynamicLibraryProductType.cpp
    Sources/pbxspec/PBX/FileType.cpp
    Sources/pbxspec/PBX/FrameworkWrapperFileType.cpp
    Sources/pbxspec/PBX/HTMLFileType.cpp
    Sources/pbxspec/PBX/Linker.cpp
    Sources/pbxspec/PBX/LinkerSpecificationLd.cpp
    Sources/pbxspec/PBX/LinkerSpecificationLibtool.cpp
    Sources/pbxspec/PBX/LinkerSpecificationResMerger.cpp
    Sources/pbxspec/PBX/MachOFileType.cpp
    Sources/pbxspec/PBX/Object.cpp
    Sources/pbxspec/PBX/PackageType.cpp
    Sources/pbxspec/PBX/PlistFileType.cpp
    Sources/pbxspec/PBX/PlugInKitPluginWrapperFileType.cpp
    Sources/pbxspec/PBX/ProductType.cpp
    Sources/pbxspec/PBX/PropertyConditionFlavor.cpp
    Sources/pbxspec/PBX/PropertyOption.cpp
    Sources/pbxspec/PBX/Specification.cpp
    Sources/pbxspec/PBX/SpotlightImporternWrapperFileType.cpp
    Sources/pbxspec/PBX/StaticLibraryProductType.cpp
    Sources/pbxspec/PBX/Tool.cpp
    Sources/pbxspec/PBX/XPCServiceWrapperFileType.cpp
    Sources/pbxspec/XC/CompilerSpecificationClang.cpp
    Sources/pbxspec/XC/StaticFrameworkWrapperFileType.cpp
    Sources/pbxspec/XC/ToolSpecificationHeadermapGenerator.cpp
    Sources/pbxspec/XC/ToolSpecificationCompilationDatabaseGenerator.cpp
    Sources/pbxspec/XC/ProductPackagingUtilityToolSpecification.cpp
    )

set(libpbxbuild_SOURCES
    Sources/pbxbuild/HeaderMap.cpp
    )

add_library(pbxsetting SHARED ${libpbxsetting_SOURCES})
target_link_libraries(pbxsetting pbxbase)

add_library(xcsdk SHARED ${libxcsdk_SOURCES})
target_link_libraries(xcsdk pbxsetting pbxbase)

add_library(pbxproj SHARED ${libpbxproj_SOURCES})
target_link_libraries(pbxproj pbxsetting pbxbase)

add_library(pbxspec SHARED ${libpbxspec_SOURCES})
target_link_libraries(pbxspec pbxsetting pbxbase)

add_library(pbxbuild SHARED ${libpbxbuild_SOURCES})
target_link_libraries(pbxbuild pbxproj pbxspec)

add_executable(plist Tools/plist.cpp)
target_link_libraries(plist pbxbase)
add_executable(sdktool Tools/sdktool.cpp)
target_link_libraries(sdktool xcsdk)
add_executable(xcconfig Tools/xcconfig.cpp)
target_link_libraries(xcconfig pbxsetting)
add_executable(hmap Tools/hmap.cpp)
target_link_libraries(hmap pbxbase pbxbuild)
add_executable(pbxtool Tools/pbxtool.cpp)
target_link_libraries(pbxtool pbxbase pbxproj pbxsetting)
add_executable(xcspec Tools/xcspec.cpp)
target_link_libraries(xcspec pbxspec)
add_executable(xctool Tools/xctool.cpp)
target_link_libraries(xctool pbxbase pbxproj)
add_executable(settings Tools/settings.cpp)
target_link_libraries(settings pbxbase pbxproj xcsdk pbxsetting pbxspec)


enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/gtest/include)
add_subdirectory(ThirdParty/gtest)

function (ADD_UNIT_GTEST NAME SOURCES)
    add_executable("${NAME}" ${SOURCES})
    target_link_libraries("${NAME}" gtest gtest_main)
    add_test("${NAME}" "${NAME}")
endfunction ()

ADD_UNIT_GTEST(test_pbxsetting Tests/pbxsetting/test_pbxsetting.cpp)
target_link_libraries(test_pbxsetting pbxsetting)
