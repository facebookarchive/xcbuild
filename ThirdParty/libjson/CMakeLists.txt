# Copyright 2013-present Facebook. All Rights Reserved.

project(libjson C)

cmake_minimum_required(VERSION 2.8)

find_package(BISON)
find_package(FLEX)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if (NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

FLEX_TARGET(JSON_SCANNER
            ${CMAKE_CURRENT_SOURCE_DIR}/tokenizer.l
            ${CMAKE_CURRENT_BINARY_DIR}/tokenizer.c)
BISON_TARGET(JSON_PARSER
             ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
             ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
ADD_FLEX_BISON_DEPENDENCY(JSON_SCANNER JSON_PARSER)

add_library(json STATIC
            json.c
            qstring.c
            ${BISON_JSON_PARSER_OUTPUTS}
            ${FLEX_JSON_SCANNER_OUTPUTS})

add_executable(jsontest jsontest.c)
target_link_libraries(jsontest json)
