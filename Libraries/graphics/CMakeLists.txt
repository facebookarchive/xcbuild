#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(graphics SHARED
            Sources/Image.cpp
            Sources/PixelFormat.cpp
            )

target_link_libraries(graphics PUBLIC ext)
target_include_directories(graphics PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
install(TARGETS graphics DESTINATION usr/lib)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(graphics PixelFormat Tests/test_PixelFormat.cpp)
endif ()
