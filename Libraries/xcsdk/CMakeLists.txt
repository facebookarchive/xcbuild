#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(xcsdk SHARED
            Sources/Environment.cpp
            Sources/SDK/Manager.cpp
            Sources/SDK/Platform.cpp
            Sources/SDK/PlatformVersion.cpp
            Sources/SDK/Product.cpp
            Sources/SDK/Target.cpp
            Sources/SDK/Toolchain.cpp
            )

target_link_libraries(xcsdk PUBLIC pbxsetting util plist)
target_include_directories(xcsdk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

add_executable(xcrun Tools/xcrun.cpp)
target_link_libraries(xcrun xcsdk util)

