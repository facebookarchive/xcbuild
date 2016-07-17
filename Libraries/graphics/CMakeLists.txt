#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(graphics SHARED
            Sources/Graphics.cpp
            )

target_include_directories(graphics PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
target_include_directories(graphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/PrivateHeaders")
install(TARGETS graphics DESTINATION usr/lib)
