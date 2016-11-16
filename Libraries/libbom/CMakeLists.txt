#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(bom
            Sources/bom.c
            Sources/bom_memory.c
            Sources/bom_tree.c
            )

target_link_libraries(bom PUBLIC)
target_include_directories(bom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
install(TARGETS bom DESTINATION usr/lib)

add_executable(lsbom Tools/lsbom.cpp)
target_link_libraries(lsbom PRIVATE util bom)
install(TARGETS lsbom DESTINATION usr/bin)

add_executable(dump_bom Tools/dump_bom.c)
target_link_libraries(dump_bom PRIVATE bom)
