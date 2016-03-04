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
    _depth  = 0;
    _parser = ::xmlReaderForMemory(reinterpret_cast<char const *>(contents.data()), contents.size(), nullptr, nullptr, XML_PARSE_NOENT | XML_PARSE_NONET);
    if (_parser == nullptr) {
        return false;
    }

    onBeginParse();

    int ret = xmlTextReaderRead(_parser);
    while (ret == 1) {
        _depth = xmlTextReaderDepth(_parser);

        int type = xmlTextReaderNodeType(_parser);
        if (type == 1 /* Start element. */) {
            std::unordered_map<std::string, std::string> attrs;

            ret = xmlTextReaderMoveToFirstAttribute(_parser);
            while (ret == 1) {
                /* Store attribute. */
                xmlChar const *name = xmlTextReaderConstName(_parser);
                xmlChar const *value = xmlTextReaderConstValue(_parser);
                attrs[std::string(reinterpret_cast<char const *>(name))] = std::string(reinterpret_cast<char const *>(value));

                ret = xmlTextReaderMoveToNextAttribute(_parser);
            }

            /* Handle error. */
            if (ret != 0) {
                break;
            }

            ret = xmlTextReaderMoveToElement(_parser);
            if (ret != 0 && ret != 1) {
                break;
            }

            /* Check for empty element. Before onStart in case of error. */
            ret = xmlTextReaderIsEmptyElement(_parser);
            if (ret != 0 && ret != 1) {
                break;
            }

            xmlChar const *name = xmlTextReaderConstName(_parser);
            onStartElement(std::string(reinterpret_cast<char const *>(name)), attrs, _depth);

            if (ret == 1) {
                /* Empty element. */
                onEndElement(std::string(reinterpret_cast<char const *>(name)), _depth);
            }
        } else if (type == 15 /* End element. */) {
            xmlChar const *name = xmlTextReaderConstName(_parser);
            onEndElement(std::string(reinterpret_cast<char const *>(name)), _depth);
        } else if (type == 3 /* Text. */) {
            xmlChar const *value = xmlTextReaderConstValue(_parser);
            onCharacterData(std::string(reinterpret_cast<char const *>(value)), _depth);
        }

        /* Handle error. */
        if (_parser == nullptr) {
            ret = -1;
            break;
        }

        ret = xmlTextReaderRead(_parser);
    }

    ::xmlFreeTextReader(_parser);
    _parser = nullptr;

    _depth = 0;
    onEndParse(ret == 0);

    return (ret == 0);
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

    _line = ::xmlTextReaderGetParserLineNumber(_parser);
    _column = ::xmlTextReaderGetParserColumnNumber(_parser);
    _error = std::string(buf);

    if (buf != sErrorMessage) {
        ::free(buf);
    }

    ::xmlFreeTextReader(_parser);
    _parser = nullptr;
}
