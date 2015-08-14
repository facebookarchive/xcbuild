// Copyright 2013-present Facebook. All Rights Reserved.

#include <plist/BaseXMLParser.h>

#include <cerrno>

using plist::BaseXMLParser;

BaseXMLParser::BaseXMLParser() :
    _parser (nullptr),
    _depth  (0)
{
}

bool BaseXMLParser::
parse(std::string const &path, error_function const &error)
{
    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if (fp == nullptr)
        return false;

    bool success = parse(fp, error);

    std::fclose(fp);

    return success;
}

bool BaseXMLParser::
parse(std::FILE *fp, error_function const &error)
{
    if (fp == nullptr) {
        errno = EBADF;
        return false;
    }

    _errorFunction = error;
    _depth         = 0;
    _parser        = ::XML_ParserCreate(nullptr);
    if (_parser == nullptr) {
        std::fclose(fp);
        errno = ENOMEM;
        return false;
    }

    ::XML_SetUserData(_parser, this);
    ::XML_SetStartElementHandler(_parser, &BaseXMLParser::StartElementHandler);
    ::XML_SetEndElementHandler(_parser, &BaseXMLParser::EndElementHandler);
    ::XML_SetCharacterDataHandler(_parser, &BaseXMLParser::CharacterDataHandler);

    onBeginParse();

    XML_Status status;
    bool       stop = false;
    while (!stop) {
        char   buf[1024];
        size_t nread;

        nread = std::fread(buf, 1, sizeof(buf), fp);
        stop = (nread <= 0);

        status = ::XML_Parse(_parser, buf, nread, stop);
        if (status != XML_STATUS_OK)
            break;
    }

    ::XML_ParserFree(_parser);

    _parser        = nullptr;
    _depth         = 0;
    _errorFunction = nullptr;

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
onStartElement(std::string const &name, string_map const &attrs, size_t depth)
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
error(std::string const &format, ...)
{
    static char const sErrorMessage[] = "syntax error";

    va_list  ap;
    char    *buf;
 
    va_start(ap, format);
    if (::vasprintf(&buf, format.c_str(), ap) < 0) {
        buf = const_cast <char *> (sErrorMessage);
    }
    va_end(ap);

    _errorFunction(::XML_GetCurrentLineNumber(_parser),
                   ::XML_GetCurrentColumnNumber(_parser),
                   buf);

    if (buf != sErrorMessage) {
        std::free(buf);
    }

    stop();
}

void XMLCALL BaseXMLParser::
StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
{
    string_map attrs;
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
