# Copyright 2013-present Facebook. All Rights Reserved.

add_library(xcassets SHARED
            Sources/Asset.cpp
            Sources/Group.cpp
            Sources/Catalog.cpp
            Sources/AppIconSet.cpp
            Sources/BrandAssets.cpp
            Sources/DataSet.cpp
            Sources/GCDashboardImage.cpp
            Sources/GCLeaderboardSet.cpp
            Sources/GCLeaderboard.cpp
            Sources/IconSet.cpp
            Sources/ImageSet.cpp
            Sources/ImageStack.cpp
            Sources/ImageStackLayer.cpp
            Sources/LaunchImage.cpp
            Sources/SpriteAtlas.cpp
            Sources/ComplicationSet.cpp
            )

target_link_libraries(xcassets PUBLIC util plist ext)
target_include_directories(xcassets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

add_executable(dump_xcassets Tools/dump_xcassets.cpp)
target_link_libraries(dump_xcassets PRIVATE xcassets)
