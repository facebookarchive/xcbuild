#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(plist SHARED
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
            Sources/Format/ABPCommon.cpp
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

find_package(LibXml2 REQUIRED)
target_include_directories(plist PRIVATE "${LIBXML2_INCLUDE_DIR}")
target_link_libraries(plist PRIVATE ${LIBXML2_LIBRARIES})
set_target_properties(plist PROPERTIES COMPILE_DEFINITIONS "${LIBXML2_DEFINITIONS}")

target_include_directories(plist PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
target_include_directories(plist PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/PrivateHeaders")
install(TARGETS plist DESTINATION usr/lib)

add_executable(plutil Tools/plutil.cpp)
target_link_libraries(plutil plist util)
install(TARGETS plutil DESTINATION usr/bin)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(plist Boolean Tests/test_Boolean.cpp)
  ADD_UNIT_GTEST(plist Real Tests/test_Real.cpp)
  ADD_UNIT_GTEST(plist String Tests/test_String.cpp)
  ADD_UNIT_GTEST(plist Encoding Tests/Format/test_Encoding.cpp)
  ADD_UNIT_GTEST(plist ASCII Tests/Format/test_ASCII.cpp)
  ADD_UNIT_GTEST(plist JSON Tests/Format/test_JSON.cpp)
  ADD_UNIT_GTEST(plist XML Tests/Format/test_XML.cpp)
endif ()
