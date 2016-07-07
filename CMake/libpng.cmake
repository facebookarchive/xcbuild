#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

function (TARGET_USE_LIBPNG TARGET)
  find_package(PNG)
  if (PNG_FOUND AND "${PNG_VERSION_STRING}" VERSION_GREATER "1.6")
    target_include_directories("${TARGET}" PRIVATE ${PNG_INCLUDE_DIRS})
    set_target_properties("${TARGET}" PROPERTIES COMPILE_DEFINITIONS "${PNG_DEFINITIONS}")
  elseif (EXISTS "${CMAKE_SOURCE_DIR}/ThirdParty/libpng")
    set(PNG_STATIC TRUE)
    set(PNG_SHARED FALSE)
    set(PNG_FRAMEWORK FALSE)
    set(PNG_TESTS FALSE)
    set(SKIP_INSTALL_ALL TRUE)
    add_subdirectory("${CMAKE_SOURCE_DIR}/ThirdParty/libpng" "${CMAKE_BINARY_DIR}/ThirdParty/libpng")

    target_link_libraries("${TARGET}" PRIVATE "png_static")
    target_include_directories("${TARGET}" PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/libpng" "${CMAKE_BINARY_DIR}/ThirdParty/libpng")
  else ()
    message(FATAL_ERROR "libpng not installed or available locally. Did you run `git submodule update --init`?")
  endif ()
endfunction ()
