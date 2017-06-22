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

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else ()
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif ()

# Output into root build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Unused language features.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# Enable all warnings.
add_compile_options(-Wall -Werror)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wno-multichar -Wno-sign-compare)
endif ()

# Enable color diagnostics.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-fcolor-diagnostics)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER "5.0")
  add_compile_options(-fdiagnostics-color)
endif ()

# Enable unit testing.
include(CTest)

if (BUILD_TESTING)
  add_subdirectory(ThirdParty/googletest/googletest)

  function (ADD_UNIT_GTEST LIBRARY NAME SOURCES)
    set(TARGET_NAME "test_${LIBRARY}_${NAME}")
    add_executable("${TARGET_NAME}" ${SOURCES})
    target_link_libraries("${TARGET_NAME}" PRIVATE "${LIBRARY}" gtest gtest_main)
    target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest/include")
    add_test(NAME "${TARGET_NAME}" COMMAND "${TARGET_NAME}")
  endfunction ()
endif ()

add_subdirectory(Libraries)
add_subdirectory(Specifications)
