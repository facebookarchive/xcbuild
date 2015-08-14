// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __plist_BaseXMLParser_h
#define __plist_BaseXMLParser_h

#include <plist/Base.h>
#include <expat.h>

namespace plist {

class BaseXMLParser {
private:
    ::XML_Parser   _parser;
    size_t         _depth;
    error_function _errorFunction;

public:
    BaseXMLParser();

protected:
    bool parse(std::string const &path, error_function const &error);
    bool parse(std::FILE *fp, error_function const &error);
    bool stop();

protected:
    inline size_t depth() const
    { return _depth; }

protected:
    virtual void onBeginParse();
    virtual void onEndParse(bool success);

protected:
    virtual void onStartElement(std::string const &name, string_map const &attrs, size_t depth);
    virtual void onEndElement(std::string const &name, size_t depth);
    virtual void onCharacterData(std::string const &cdata, size_t depth);

protected:
    void error(std::string const &format, ...);

private:
    static void XMLCALL StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
    static void XMLCALL EndElementHandler(void *userData, const XML_Char *name); 
    static void XMLCALL CharacterDataHandler(void *userData, const XML_Char *s, int len); 
};

}

#endif  // !__plist_BaseXMLParser_h
