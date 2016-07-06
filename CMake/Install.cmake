#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

set(XCBUILD_INSTALL_DEVELOPER_DIR Developer CACHE STRING "Install path for xcbuild support files")

# XCBUILD_INSTALL_TARGET(foo bar qix) installs the CMake targets 'foo', 'bar',
# and 'qix' into the xcbuild Developer dir (specified by
# XCBUILD_INSTALL_DEVELOPER_DIR).
function (XCBUILD_INSTALL_TARGET)
  cmake_parse_arguments("" "" "" "" ${ARGN})
  set(TARGETS ${_UNPARSED_ARGUMENTS})
  # ARCHIVE is deliberately omitted; intentionally do not install static
  # libraries (including Windows import libraries).
  # TODO(strager): Handle FRAMEWORK and BUNDLE targets.
  install(TARGETS ${TARGETS}
          RUNTIME DESTINATION ${XCBUILD_INSTALL_DEVELOPER_DIR}/usr/bin
          LIBRARY DESTINATION ${XCBUILD_INSTALL_DEVELOPER_DIR}/usr/lib)
endfunction ()
