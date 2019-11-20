#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
#

add_library(plist
            Sources/ObjectType.cpp
            Sources/Array.cpp
            Sources/Boolean.cpp
            Sources/Data.cpp
            Sources/Date.cpp
            Sources/Dictionary.cpp
            Sources/Integer.cpp
            Sources/Null.cpp
            Sources/Object.cpp
            Sources/Real.cpp
            Sources/String.cpp
            Sources/UID.cpp
            #
            Sources/Base64.cpp
            Sources/rfc4648.c
            Sources/ISODate.cpp
            Sources/UnixTime.cpp
            #
            Sources/Keys/Unpack.cpp
            #
            Sources/Format/Encoding.cpp
            Sources/Format/unicode.c
            #
            Sources/Format/BaseXMLParser.cpp
            Sources/Format/XMLParser.cpp
            Sources/Format/XMLWriter.cpp
            Sources/Format/XML.cpp
            #
            Sources/Format/SimpleXMLParser.cpp
            Sources/Format/SimpleXML.cpp
            #
            Sources/Format/ABPContext.cpp
            Sources/Format/ABPReader.cpp
            Sources/Format/ABPWriter.cpp
            Sources/Format/Binary.cpp
            #
            Sources/Format/ASCIIPListLexer.cpp
            Sources/Format/ASCIIParser.cpp
            Sources/Format/ASCIIWriter.cpp
            Sources/Format/ASCII.cpp
            #
            Sources/Format/JSONParser.cpp
            Sources/Format/JSONWriter.cpp
            Sources/Format/JSON.cpp
            #
            Sources/Format/Any.cpp
            )

target_link_libraries(plist PRIVATE util)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
  if ("${CMAKE_CXX_PLATFORM_ID}" STREQUAL "MinGW")
    # MinGW is missing the library for XmlLite.
    target_link_libraries(plist PRIVATE ole32)
  else ()
    target_link_libraries(plist PRIVATE xmllite Ole32)
  endif ()
else ()
  find_package(LibXml2 REQUIRED)
  if ("${CMAKE_SYSTEM_NAME}" MATCHES "OpenBSD")
    target_include_directories(plist PRIVATE "${LIBXML2_INCLUDE_DIRS}")
  else ()
    target_include_directories(plist PRIVATE "${LIBXML2_INCLUDE_DIR}")
  endif ()
  target_compile_definitions(plist PRIVATE "${LIBXML2_DEFINITIONS}")
  target_link_libraries(plist PRIVATE ${LIBXML2_LIBRARIES})
endif ()

target_include_directories(plist PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
target_include_directories(plist PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/PrivateHeaders")
install(TARGETS plist DESTINATION usr/lib)

add_executable(plutil Tools/plutil.cpp)
target_link_libraries(plutil PRIVATE plist process util)
install(TARGETS plutil DESTINATION usr/bin)

add_executable(PlistBuddy Tools/PlistBuddy.cpp)
target_link_libraries(PlistBuddy PRIVATE plist util)
install(TARGETS PlistBuddy DESTINATION usr/bin)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  # TODO
else ()
  set(LINENOISE_ROOT "${CMAKE_SOURCE_DIR}/ThirdParty/linenoise")
  set(LINENOISE_SOURCE "${LINENOISE_ROOT}/linenoise.c")
  if (EXISTS "${LINENOISE_SOURCE}")
    add_library(linenoise "${LINENOISE_SOURCE}")
    target_include_directories(linenoise PUBLIC "${LINENOISE_ROOT}")
    if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
      target_compile_definitions(linenoise PRIVATE _GNU_SOURCE)
    endif ()

    target_link_libraries(PlistBuddy PRIVATE linenoise)
    target_compile_definitions(PlistBuddy PRIVATE HAVE_LINENOISE)
  endif ()
endif ()

if (BUILD_TESTING)
  ADD_UNIT_GTEST(plist Boolean Tests/test_Boolean.cpp)
  ADD_UNIT_GTEST(plist Real Tests/test_Real.cpp)
  ADD_UNIT_GTEST(plist String Tests/test_String.cpp)
  ADD_UNIT_GTEST(plist Encoding Tests/Format/test_Encoding.cpp)
  ADD_UNIT_GTEST(plist ASCII Tests/Format/test_ASCII.cpp)
  ADD_UNIT_GTEST(plist Binary Tests/Format/test_Binary.cpp)
  ADD_UNIT_GTEST(plist JSON Tests/Format/test_JSON.cpp)
  ADD_UNIT_GTEST(plist XML Tests/Format/test_XML.cpp)
endif ()
