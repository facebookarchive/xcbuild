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
#
# If USER is specified, also install the targets into CMAKE_INSTALL_PREFIX.
# Mutually exclusive with USER.
#
# If USER_ALIAS is specified, also install xcrun into CMAKE_INSTALL_PREFIX
# renamed for each target. All targets must be executables. Mutually exclusive
# with USER.
#
# USER_ALIAS allows users to easily access Developer dir tools without adding
# Developer/usr/bin to $PATH. For example, XCBUILD_INSTALL_ALIAS(plutil USER)
# will install xcrun into /usr/local/bin/plutil. Because xcrun inspects its file
# name at runtime, xcrun will find and invoke ${DEVELOPER_DIR}/usr/bin/plutil.
function (XCBUILD_INSTALL_TARGET)
  cmake_parse_arguments("" "USER;USER_ALIAS" "" "" ${ARGN})
  if (${_USER} AND ${_USER_ALIAS})
    message(AUTHOR_WARNING "XCBUILD_INSTALL_TARGET was called with mutually exclusive options: USER and USER_ALIAS")
    set(_USER_ALIAS False)
  endif ()
  set(TARGETS ${_UNPARSED_ARGUMENTS})
  # ARCHIVE is deliberately omitted; intentionally do not install static
  # libraries (including Windows import libraries).
  # TODO(strager): Handle FRAMEWORK and BUNDLE targets.
  install(TARGETS ${TARGETS}
          RUNTIME DESTINATION ${XCBUILD_INSTALL_DEVELOPER_DIR}/usr/bin
          LIBRARY DESTINATION ${XCBUILD_INSTALL_DEVELOPER_DIR}/usr/lib)
  if (${_USER})
    install(TARGETS ${TARGETS}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)
  endif ()
  if (${_USER_ALIAS})
    foreach (TARGET ${TARGETS})
      get_target_property(TYPE ${TARGET} TYPE)
      if (${TYPE} STREQUAL EXECUTABLE)
        # NOTE(strager): Assumes XCBUILD_INSTALL_TARGET(xcrun USER) was (or will
        # be) called.
        XCBUILD_INSTALL_PROGRAM_ALIAS(TARGET ${TARGET} ALIASED_TARGET xcrun DESTINATION bin)
      else ()
        message(AUTHOR_WARNING "XCBUILD_INSTALL_DEVELOPER_DIR cannot install non-executable (${TYPE}) target ${TARGET} with USER_ALIAS")
      endif ()
    endforeach ()
  endif ()
endfunction ()

# XCBUILD_INSTALL_PROGRAM_ALIAS(ALIASED_TARGET a DESTINATION d TARGET t)
# installs the 'a' target with the name of the 't' target in the 'd' directory.
#
# Assumes install(TARGETS t DESTINATION d) was (or will be) called.
function (XCBUILD_INSTALL_PROGRAM_ALIAS)
  # There are several ways to implement this, each with their own problems:
  #
  # * install(TARGETS). This does not support the RENAME option.
  #   TODO(strager): Investigate this solution with hackily-duplicated targets.
  #
  # * install(PROGRAMS $<TARGET_FILE:>). install(PROGRAMS) does not post-process
  #   the executable. RENAME does not support generator expressions.
  #
  # * install(CODE "file(INSTALL ... FILES
  #           \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/..."\"). This requires
  #   the aliased target be previously install()ed. install(CODE) does not
  #   support generator expressions.
  #
  # * cmake -E create_symlink, install(PROGRAMS). Symlinks require the aliased
  #   target to be eventually installed in the same destination. This does not
  #   work on systems which do not support symlinks (e.g. NTFS (Windows)).
  #
  # Because xcbuild currently does not support Windows, use symlinks.
  set(_ALIASED_TARGET)
  set(_DESTINATION)
  set(_TARGET)
  cmake_parse_arguments("" "" "ALIASED_TARGET;DESTINATION;TARGET" "" ${ARGN})
  if (${_UNPARSED_ARGUMENTS})
    message(AUTHOR_WARNING "XCBUILD_INSTALL_PROGRAM_ALIAS does not recognize the following arguments: ${_UNPARSED_ARGUMENTS}")
  endif ()
  if (${_ALIASED_TARGET} STREQUAL "")
    message(SEND_ERROR "XCBUILD_INSTALL_PROGRAM_ALIAS is missing the required ALIASED_TARGET option")
    return ()
  endif ()
  if (${_DESTINATION} STREQUAL "")
    message(SEND_ERROR "XCBUILD_INSTALL_PROGRAM_ALIAS is missing the required DESTINATION option")
    return ()
  endif ()
  if (${_TARGET} STREQUAL "")
    message(SEND_ERROR "XCBUILD_INSTALL_PROGRAM_ALIAS is missing the required TARGET option")
    return ()
  endif ()
  set(TEMP_PATH ${CMAKE_CURRENT_BINARY_DIR}/xcbuild-alias/${ALIASED_TARGET})
  set(TEMP_FILE ${TEMP_PATH}/$<TARGET_FILE_NAME:${_TARGET}>)
  add_custom_command(TARGET ${_TARGET}
                     COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_NAME:${_ALIASED_TARGET}> ${TEMP_PATH}
                     COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:${_ALIASED_TARGET}> ${TEMP_FILE})
  install(PROGRAMS ${TEMP_FILE} DESTINATION ${_DESTINATION})
endfunction ()
