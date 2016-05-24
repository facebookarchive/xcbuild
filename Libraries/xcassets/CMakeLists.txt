# Copyright 2013-present Facebook. All Rights Reserved.

add_library(xcassets SHARED
            Sources/Insets.cpp
            Sources/Resizing.cpp
            Sources/ContentReference.cpp
            Sources/FullyQualifiedName.cpp
            Sources/MatchingStyle.cpp
            Sources/TemplateRenderingIntent.cpp
            Sources/Slot/DeviceSubtype.cpp
            Sources/Slot/GraphicsFeatureSet.cpp
            Sources/Slot/Idiom.cpp
            Sources/Slot/Orientation.cpp
            Sources/Slot/LaunchImageExtent.cpp
            Sources/Slot/MemoryRequirement.cpp
            Sources/Slot/Scale.cpp
            Sources/Slot/SizeClass.cpp
            Sources/Slot/WatchIconRole.cpp
            Sources/Asset/Asset.cpp
            Sources/Asset/AppIconSet.cpp
            Sources/Asset/BrandAssets.cpp
            Sources/Asset/Catalog.cpp
            Sources/Asset/ComplicationSet.cpp
            Sources/Asset/DataSet.cpp
            Sources/Asset/GCDashboardImage.cpp
            Sources/Asset/GCLeaderboardSet.cpp
            Sources/Asset/GCLeaderboard.cpp
            Sources/Asset/Group.cpp
            Sources/Asset/IconSet.cpp
            Sources/Asset/ImageSet.cpp
            Sources/Asset/ImageStack.cpp
            Sources/Asset/ImageStackLayer.cpp
            Sources/Asset/LaunchImage.cpp
            Sources/Asset/SpriteAtlas.cpp
            )

target_link_libraries(xcassets PUBLIC util plist ext)
target_include_directories(xcassets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

add_executable(dump_xcassets Tools/dump_xcassets.cpp)
target_link_libraries(dump_xcassets PRIVATE xcassets)

ADD_UNIT_GTEST(xcassets FullyQualifiedName Tests/test_FullyQualifiedName.cpp)
ADD_UNIT_GTEST(xcassets IconSet Tests/test_IconSet.cpp)
ADD_UNIT_GTEST(xcassets Scale Tests/test_Scale.cpp)
