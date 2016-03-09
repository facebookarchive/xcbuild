# Copyright 2013-present Facebook. All Rights Reserved.

add_library(acdriver SHARED
            Sources/Options.cpp
            Sources/Driver.cpp
            )

target_link_libraries(acdriver PUBLIC util ext)
target_include_directories(acdriver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

add_executable(actool Tools/actool.cpp)
target_link_libraries(actool PRIVATE acdriver)

