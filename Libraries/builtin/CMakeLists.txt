#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(builtin SHARED
            Sources/Driver.cpp
            Sources/Registry.cpp
            #
            Sources/copy/Options.cpp
            Sources/copy/Driver.cpp
            #
            Sources/copyPlist/Options.cpp
            Sources/copyPlist/Driver.cpp
            #
            Sources/copyStrings/Options.cpp
            Sources/copyStrings/Driver.cpp
            #
            Sources/copyTiff/Options.cpp
            Sources/copyTiff/Driver.cpp
            #
            Sources/infoPlistUtility/Options.cpp
            Sources/infoPlistUtility/Driver.cpp
            #
            Sources/lsRegisterURL/Options.cpp
            Sources/lsRegisterURL/Driver.cpp
            #
            Sources/productPackagingUtility/Options.cpp
            Sources/productPackagingUtility/Driver.cpp
            #
            Sources/validationUtility/Options.cpp
            Sources/validationUtility/Driver.cpp
            #
            Sources/embeddedBinaryValidationUtility/Options.cpp
            Sources/embeddedBinaryValidationUtility/Driver.cpp
            )

find_library(CORE_FOUNDATION CoreFoundation)
if ("${CORE_FOUNDATION}" STREQUAL "CORE_FOUNDATION-NOTFOUND")
  set(CORE_FOUNDATION "")
endif ()

find_library(CORE_SERVICES CoreServices)
if ("${CORE_SERVICES}" STREQUAL "CORE_SERVICES-NOTFOUND")
  set(CORE_SERVICES "")
endif ()

target_link_libraries(builtin PUBLIC util plist pbxsetting ${CORE_FOUNDATION} ${CORE_SERVICES})
target_include_directories(builtin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

