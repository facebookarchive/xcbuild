# Copyright 2013-present Facebook. All Rights Reserved.

add_library(bom SHARED
            Sources/bom.c
            Sources/bom_memory.c
            Sources/bom_tree.c
            )

target_link_libraries(bom PUBLIC)
target_include_directories(bom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

