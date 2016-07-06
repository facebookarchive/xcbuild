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
XCBUILD_INSTALL_TARGET(builtin)

add_executable(builtin-copy Tools/copy.cpp)
target_link_libraries(builtin-copy builtin)
XCBUILD_INSTALL_TARGET(builtin-copy)

add_executable(builtin-copyPlist Tools/copyPlist.cpp)
target_link_libraries(builtin-copyPlist builtin)
XCBUILD_INSTALL_TARGET(builtin-copyPlist)

add_executable(builtin-copyStrings Tools/copyStrings.cpp)
target_link_libraries(builtin-copyStrings builtin)
XCBUILD_INSTALL_TARGET(builtin-copyStrings)

add_executable(builtin-copyTiff Tools/copyTiff.cpp)
target_link_libraries(builtin-copyTiff builtin)
XCBUILD_INSTALL_TARGET(builtin-copyTiff)

add_executable(builtin-infoPlistUtility Tools/infoPlistUtility.cpp)
target_link_libraries(builtin-infoPlistUtility builtin)
XCBUILD_INSTALL_TARGET(builtin-infoPlistUtility)

add_executable(builtin-lsRegisterURL Tools/lsRegisterURL.cpp)
target_link_libraries(builtin-lsRegisterURL builtin)
XCBUILD_INSTALL_TARGET(builtin-lsRegisterURL)

add_executable(builtin-productPackagingUtility Tools/productPackagingUtility.cpp)
target_link_libraries(builtin-productPackagingUtility builtin)
XCBUILD_INSTALL_TARGET(builtin-productPackagingUtility)

add_executable(builtin-validationUtility Tools/validationUtility.cpp)
target_link_libraries(builtin-validationUtility builtin)
XCBUILD_INSTALL_TARGET(builtin-validationUtility)

add_executable(builtin-embeddedBinaryValidationUtility Tools/embeddedBinaryValidationUtility.cpp)
target_link_libraries(builtin-embeddedBinaryValidationUtility builtin)
XCBUILD_INSTALL_TARGET(builtin-embeddedBinaryValidationUtility)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(builtin copyStrings Tests/test_copyStrings.cpp)
endif ()
