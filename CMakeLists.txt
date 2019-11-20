#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
#

project(xcbuild C CXX)

cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0054 NEW)

set(BUILD_SHARED_LIBS True CACHE BOOL "Build shared libraries." FORCE)

# Platform options.
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  # Export all symbols to avoid needing a `.def` file or `__dllexport`.
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)

  if ("${CMAKE_CXX_PLATFORM_ID}" STREQUAL "MinGW")
    # Use MinGW formatting rather than MSVCRT for C99 support.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__USE_MINGW_ANSI_STDIO=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_MINGW_ANSI_STDIO=1")
  endif ()
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(CMAKE_MACOSX_RPATH True)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif ()

# Output into root build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Language version.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" OR "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  # C++ is by default. C99 is not supported.
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else ()
  message(WARNING "Compiler not supported to enable C++11.")
endif ()

# Unused language features.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" OR "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")
else ()
  message(WARNING "Compiler not supported to disable exceptions and RTTI.")
endif ()

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC") OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif ()

# Enable all warnings.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" OR "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  add_compile_options(/D_CRT_SECURE_NO_WARNINGS) # TODO: /Wall /WX
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wall -Werror)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wno-multichar -Wno-sign-compare)
  endif ()
else ()
  message(WARNING "Compiler not supported to enable warnings.")
endif ()

# Enable color diagnostics.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  add_compile_options(-fcolor-diagnostics)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER "5.0")
  add_compile_options(-fdiagnostics-color)
endif ()

# Port BSD gcc
if ("${CMAKE_SYSTEM_NAME}" MATCHES "FreeBSD|OpenBSD")
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-D_GLIBCXX_USE_C99 -D_GLIBCXX_USE_C99_MATH -D_GLIBCXX_USE_C99_MATH_TR1 -D_GLIBCXX_USE_CXX11_ABI=1)
  endif ()
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER "5.0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lstdc++")
  endif ()
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
