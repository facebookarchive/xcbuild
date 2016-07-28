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
            Sources/Format/PNG.cpp
            )

find_library(CORE_FOUNDATION CoreFoundation)
if ("${CORE_FOUNDATION}" STREQUAL "CORE_FOUNDATION-NOTFOUND")
  set(CORE_FOUNDATION "")
endif ()

find_library(CORE_GRAPHICS CoreGraphics)
if ("${CORE_GRAPHICS}" STREQUAL "CORE_GRAPHICS-NOTFOUND")
  set(CORE_GRAPHICS "")
endif ()

find_library(IMAGE_IO ImageIO)
if ("${IMAGE_IO}" STREQUAL "IMAGE_IO-NOTFOUND")
  set(IMAGE_IO "")
endif ()

set(PNG_STATIC ON)
find_package(PNG)
if (NOT "${PNG_FOUND}")
  set(PNG_LIBRARIES "")
  set(PNG_INCLUDE_DIRS "")
  set(PNG_DEFINITIONS "")
endif ()

find_package(ZLIB REQUIRED)

target_link_libraries(graphics PUBLIC ext ${ZLIB_LIBRARIES} ${PNG_LIBRARIES} ${CORE_FOUNDATION} ${CORE_GRAPHICS} ${IMAGE_IO})
target_include_directories(graphics PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
target_include_directories(graphics PRIVATE ${PNG_INCLUDE_DIRS})
target_include_directories(graphics PRIVATE "${ZLIB_INCLUDE_DIR}")
set_target_properties(graphics PROPERTIES COMPILE_DEFINITIONS "${PNG_DEFINITIONS}")
install(TARGETS graphics DESTINATION usr/lib)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(graphics PixelFormat Tests/test_PixelFormat.cpp)
  ADD_UNIT_GTEST(graphics PNG Tests/test_PNG.cpp)
endif ()
