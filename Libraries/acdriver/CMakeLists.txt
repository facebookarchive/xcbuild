# Copyright 2013-present Facebook. All Rights Reserved.

add_library(acdriver SHARED
            Sources/Options.cpp
            Sources/Driver.cpp
            Sources/Output.cpp
            Sources/Result.cpp
            Sources/VersionAction.cpp
            Sources/CompileAction.cpp
            Sources/ContentsAction.cpp
            )

target_link_libraries(acdriver PUBLIC xcassets util plist ext)
target_include_directories(acdriver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

add_executable(actool Tools/actool.cpp)
target_link_libraries(actool PRIVATE acdriver)

ADD_UNIT_GTEST(acdriver Options Tests/test_Options.cpp)
ADD_UNIT_GTEST(acdriver Output Tests/test_Output.cpp)
ADD_UNIT_GTEST(acdriver Result Tests/test_Result.cpp)
