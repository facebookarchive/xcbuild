#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
#

add_library(util
            Sources/FSUtil.cpp
            Sources/Filesystem.cpp
            Sources/DefaultFilesystem.cpp
            Sources/MemoryFilesystem.cpp
            Sources/Permissions.cpp
            Sources/Absolute.cpp
            Sources/Relative.cpp
            Sources/Unix.cpp
            Sources/Windows.cpp
            #
            Sources/Options.cpp
            #
            Sources/Escape.cpp
            Sources/Wildcard.cpp
            #
            Sources/md5.c
            )

target_link_libraries(util PUBLIC ext)
target_include_directories(util PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
install(TARGETS util DESTINATION usr/lib)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(util MemoryFilesystem Tests/test_MemoryFilesystem.cpp)
  ADD_UNIT_GTEST(util FSUtil Tests/test_FSUtil.cpp)
  ADD_UNIT_GTEST(util Wildcard Tests/test_Wildcard.cpp)
  ADD_UNIT_GTEST(util Escape Tests/test_Escape.cpp)
  ADD_UNIT_GTEST(util Unix Tests/test_Unix.cpp)
  ADD_UNIT_GTEST(util Windows Tests/test_Windows.cpp)
endif ()
