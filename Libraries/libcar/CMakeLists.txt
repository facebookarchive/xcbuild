#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(car SHARED
            Sources/Reader.cpp
            Sources/AttributeList.cpp
            Sources/Facet.cpp
            Sources/Rendition.cpp
            Sources/car_format.c
            Sources/Writer.cpp
            )

find_package(ZLIB REQUIRED)
target_include_directories(car PRIVATE "${ZLIB_INCLUDE_DIR}")
target_link_libraries(car PRIVATE ${ZLIB_LIBRARIES})

set(LZFSE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/ThirdParty/lzfse")
if (IS_DIRECTORY "${LZFSE_SOURCE_DIR}")
  set(LZFSE_BINARY_DIR "${PROJECT_BINARY_DIR}/lzfse") 
  set(LZFSE_BUNDLE_MODE FALSE)
  add_subdirectory("${LZFSE_SOURCE_DIR}" "${LZFSE_BINARY_DIR}" EXCLUDE_FROM_ALL)

  target_compile_definitions(car PRIVATE HAVE_LZFSE=1)
  target_link_libraries(car PRIVATE lzfse)
  target_include_directories(car PRIVATE "${LZFSE_SOURCE_DIR}/src")
endif ()

find_library(COMPRESSION compression)
if (NOT "${COMPRESSION}" STREQUAL "COMPRESSION-NOTFOUND")
  target_compile_definitions(car PRIVATE HAVE_COMPRESSION=1)
  target_link_libraries(car PRIVATE "${COMPRESSION}") 
endif ()

target_link_libraries(car PUBLIC ext bom)
target_include_directories(car PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
install(TARGETS car DESTINATION usr/lib)

add_executable(dump_car Tools/dump_car.cpp)
target_link_libraries(dump_car PRIVATE car graphics)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(car Facet Tests/test_Facet.cpp)
  ADD_UNIT_GTEST(car Rendition Tests/test_Rendition.cpp)
  ADD_UNIT_GTEST(car AttributeList Tests/test_AttributeList.cpp)
  ADD_UNIT_GTEST(car Writer Tests/test_Writer.cpp)
endif ()
