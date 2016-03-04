#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(xcworkspace SHARED
            Sources/XC/Workspace.cpp
            Sources/XC/FileRef.cpp
            Sources/XC/Group.cpp
            Sources/XC/GroupItem.cpp
            )

target_link_libraries(xcworkspace PUBLIC pbxsetting util plist)
target_include_directories(xcworkspace PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
