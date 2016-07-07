#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

function (TARGET_USE_LIBXML2 TARGET)
  find_package(LibXml2 REQUIRED)
  if (LIBXML2_FOUND)
    target_include_directories("${TARGET}" PRIVATE "${LIBXML2_INCLUDE_DIR}")
    target_link_libraries("${TARGET}" PRIVATE ${LIBXML2_LIBRARIES})
    set_target_properties("${TARGET}" PROPERTIES COMPILE_DEFINITIONS "${LIBXML2_DEFINITIONS}")
  else ()
    message(FATAL_ERROR "libxml2 is required.")
  endif ()
endfunction ()
