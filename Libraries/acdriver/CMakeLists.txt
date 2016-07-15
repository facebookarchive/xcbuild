#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(acdriver SHARED
            Sources/Options.cpp
            Sources/Driver.cpp
            Sources/Output.cpp
            Sources/Result.cpp
            Sources/VersionAction.cpp
            Sources/CompileAction.cpp
            Sources/CompileOutput.cpp
            Sources/Compile/AppIconSet.cpp
            Sources/Compile/BrandAssets.cpp
            Sources/Compile/ComplicationSet.cpp
            Sources/Compile/DataSet.cpp
            Sources/Compile/GCDashboardImage.cpp
            Sources/Compile/GCLeaderboard.cpp
            Sources/Compile/GCLeaderboardSet.cpp
            Sources/Compile/IconSet.cpp
            Sources/Compile/ImageSet.cpp
            Sources/Compile/ImageStack.cpp
            Sources/Compile/ImageStackLayer.cpp
            Sources/Compile/LaunchImage.cpp
            Sources/Compile/SpriteAtlas.cpp
            Sources/ContentsAction.cpp
            )

find_package(PNG REQUIRED)

target_link_libraries(acdriver PUBLIC xcassets util plist dependency ext bom car ${PNG_LIBRARIES})
target_include_directories(acdriver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
target_include_directories(acdriver PRIVATE ${PNG_INCLUDE_DIRS})
set_target_properties(acdriver PROPERTIES COMPILE_DEFINITIONS "${PNG_DEFINITIONS}")

install(TARGETS acdriver DESTINATION usr/lib)

add_executable(actool Tools/actool.cpp)
target_link_libraries(actool PRIVATE acdriver)
install(TARGETS actool DESTINATION usr/bin)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(acdriver Options Tests/test_Options.cpp)
  ADD_UNIT_GTEST(acdriver Output Tests/test_Output.cpp)
  ADD_UNIT_GTEST(acdriver Result Tests/test_Result.cpp)
  ADD_UNIT_GTEST(acdriver CompileOutput Tests/test_CompileOutput.cpp)
endif ()
