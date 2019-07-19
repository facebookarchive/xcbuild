#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
#

add_library(xcsdk
            Sources/Configuration.cpp
            Sources/Environment.cpp
            Sources/SDK/Manager.cpp
            Sources/SDK/Platform.cpp
            Sources/SDK/PlatformVersion.cpp
            Sources/SDK/Product.cpp
            Sources/SDK/Target.cpp
            Sources/SDK/Toolchain.cpp
            )

target_link_libraries(xcsdk PUBLIC pbxsetting process util plist)
target_include_directories(xcsdk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
install(TARGETS xcsdk DESTINATION usr/lib)

add_executable(xcrun Tools/xcrun.cpp)
target_link_libraries(xcrun xcsdk util)
install(TARGETS xcrun DESTINATION usr/bin)

add_executable(xcode-select Tools/xcode-select.cpp)
target_link_libraries(xcode-select xcsdk util)
install(TARGETS xcode-select DESTINATION usr/bin)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(xcsdk PlatformVersion Tests/test_PlatformVersion.cpp)
  ADD_UNIT_GTEST(xcsdk Toolchain Tests/test_Toolchain.cpp)
  ADD_UNIT_GTEST(xcsdk Configuration Tests/test_Configuration.cpp)
  ADD_UNIT_GTEST(xcsdk Manager Tests/test_Manager.cpp)
endif ()
