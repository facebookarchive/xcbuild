#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_subdirectory(dump)
add_subdirectory(builtin)

add_executable(plutil plutil.cpp)
target_link_libraries(plutil plist util)

add_executable(xcrun xcrun.cpp)
target_link_libraries(xcrun xcsdk util)

add_executable(xcbuild xcbuild.cpp)
target_link_libraries(xcbuild xcdriver)

add_custom_command(TARGET xcbuild
                   COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:xcbuild> xcodebuild
                   DEPENDS $<TARGET_FILE:xcbuild>)

add_executable(ninja-dependency-info ninja-dependency-info.cpp)
target_link_libraries(ninja-dependency-info dependency util)

