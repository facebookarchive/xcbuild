#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_executable(dump_xcconfig xcconfig.cpp)
target_link_libraries(dump_xcconfig pbxsetting util)

add_executable(dump_hmap hmap.cpp)
target_link_libraries(dump_hmap util plist pbxbuild)

add_executable(dump_xcodeproj xcodeproj.cpp)
target_link_libraries(dump_xcodeproj util plist pbxproj xcscheme pbxsetting)

add_executable(dump_xcspec xcspec.cpp)
target_link_libraries(dump_xcspec pbxspec)

add_executable(dump_xcworkspace xcworkspace.cpp)
target_link_libraries(dump_xcworkspace util plist pbxproj xcscheme xcworkspace)
