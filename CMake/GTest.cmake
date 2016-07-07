#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

# Enable unit testing.
include(CTest)

if (BUILD_TESTING)
  if (EXISTS "${CMAKE_SOURCE_DIR}/ThirdParty/googletest/")
    add_subdirectory("${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest" "${CMAKE_BINARY_DIR}/ThirdParty/googletest")

    function (ADD_UNIT_GTEST LIBRARY NAME SOURCES)
      set(TARGET "test_${LIBRARY}_${NAME}")
      add_executable("${TARGET}" ${SOURCES})
      target_link_libraries("${TARGET}" PRIVATE "${LIBRARY}" gtest gtest_main)
      target_include_directories("${TARGET}" PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest/include")
      add_test(NAME "${TARGET}" COMMAND "${TARGET}")
    endfunction ()
  else ()
    message(FATAL_ERROR "Testing requested but GTest not available. Did you run `git submodule update --init`?")
  endif ()
endif ()
