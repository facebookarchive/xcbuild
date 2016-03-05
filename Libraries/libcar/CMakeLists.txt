# Copyright 2013-present Facebook. All Rights Reserved.

add_library(car SHARED
            Sources/car.c
            Sources/car_attribute.c
            Sources/car_facet.c
            Sources/car_rendition.c
            )

find_package(ZLIB REQUIRED)
target_include_directories(car PRIVATE "${ZLIB_INCLUDE_DIR}")
target_link_libraries(car PRIVATE ${ZLIB_LIBRARIES})

target_link_libraries(car PUBLIC bom compression)
target_include_directories(car PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

