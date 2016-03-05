#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

project(xcbuild C CXX)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_MACOSX_RPATH 1)

# Output into root build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# Enable all warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
endif ()

# Enable color diagnostics.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER "5.0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif ()
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_C_COMPILER_VERSION}" VERSION_GREATER "5.0")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
endif ()

# Enable unit testing.
enable_testing()
add_subdirectory(ThirdParty/googletest/googletest)

function (ADD_UNIT_GTEST LIBRARY NAME SOURCES)
    set(TARGET_NAME "test_${LIBRARY}_${NAME}")
    add_executable("${TARGET_NAME}" ${SOURCES})
    target_link_libraries("${TARGET_NAME}" PRIVATE "${LIBRARY}" gtest gtest_main)
    target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest/include")
    add_test("${TARGET_NAME}" "${TARGET_NAME}")
endfunction ()

add_subdirectory(Libraries)

