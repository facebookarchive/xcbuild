/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/BaseXMLParser.h>

#include <cerrno>
#include <cstdlib>

using plist::Format::BaseXMLParser;

BaseXMLParser::BaseXMLParser() :
    _parser (nullptr),
    _depth  (0)
{
}

bool BaseXMLParser::
parse(std::vector<uint8_t> const &contents)
{
    _depth         = 0;
    _parser        = ::XML_ParserCreate(nullptr);
    if (_parser == nullptr) {
        return false;
    }

    ::XML_SetUserData(_parser, this);
    ::XML_SetStartElementHandler(_parser, &BaseXMLParser::StartElementHandler);
    ::XML_SetEndElementHandler(_parser, &BaseXMLParser::EndElementHandler);
    ::XML_SetCharacterDataHandler(_parser, &BaseXMLParser::CharacterDataHandler);

    onBeginParse();

    XML_Status status = ::XML_Parse(_parser, reinterpret_cast<char const *>(contents.data()), contents.size(), true);
    ::XML_ParserFree(_parser);

    _parser        = nullptr;
    _depth         = 0;

    onEndParse(status == XML_STATUS_OK);

    return (status == XML_STATUS_OK);
}

bool BaseXMLParser::
stop()
{
    if (_parser == nullptr)
        return false;

    return ::XML_StopParser(_parser, XML_FALSE) == XML_STATUS_OK;
}

void BaseXMLParser::
onBeginParse()
{
}

void BaseXMLParser::
onEndParse(bool success)
{
}

void BaseXMLParser::
onStartElement(std::string const &name, std::unordered_map<std::string, std::string> const &attrs, size_t depth)
{
}

void BaseXMLParser::
onEndElement(std::string const &name, size_t depth)
{
}

void BaseXMLParser::
onCharacterData(std::string const &cdata, size_t depth)
{
}

void BaseXMLParser::
error(std::string format, ...)
{
    static char const sErrorMessage[] = "syntax error";

    va_list  ap;
    char    *buf;

    va_start(ap, format);
    if (::vasprintf(&buf, format.c_str(), ap) < 0) {
        buf = const_cast <char *> (sErrorMessage);
    }
    va_end(ap);

    _line = ::XML_GetCurrentLineNumber(_parser);
    _column = ::XML_GetCurrentColumnNumber(_parser);
    _error = std::string(buf);

    if (buf != sErrorMessage) {
        ::free(buf);
    }

    stop();
}

void XMLCALL BaseXMLParser::
StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
{
    std::unordered_map<std::string, std::string> attrs;
    auto       self = reinterpret_cast <BaseXMLParser *> (userData);

    for (const XML_Char **attsp = atts; *attsp != nullptr; attsp += 2) {
        attrs[attsp[0]] = attsp[1];
    }
    self->onStartElement(name, attrs, self->_depth);
    self->_depth++;
}

void XMLCALL BaseXMLParser::
EndElementHandler(void *userData, const XML_Char *name)
{
    auto self = reinterpret_cast <BaseXMLParser *> (userData);
    self->_depth--;
    self->onEndElement(name, self->_depth);
}

void XMLCALL BaseXMLParser::
CharacterDataHandler(void *userData, const XML_Char *s, int len)
{
    auto self = reinterpret_cast <BaseXMLParser *> (userData);
    self->onCharacterData(std::string(s, len), self->_depth);
}
