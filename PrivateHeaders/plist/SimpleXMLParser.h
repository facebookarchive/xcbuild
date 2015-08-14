// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __plist_SimpleXMLParser_h
#define __plist_SimpleXMLParser_h

#include <plist/BaseXMLParser.h>
#include <plist/Objects.h>

namespace plist {

class SimpleXMLParser : public BaseXMLParser {
private:
    Dictionary                 *_root;
    Dictionary                 *_current;
    std::vector <Dictionary *>  _stack;

public:
    SimpleXMLParser();

public:
    Dictionary *parse(std::string const &path, error_function const &error);
    Dictionary *parse(std::FILE *fp, error_function const &error);

private:
    virtual void onBeginParse();
    virtual void onEndParse(bool success);

private:
    void onStartElement(std::string const &name, string_map const &attrs, size_t);
    void onEndElement(std::string const &name, size_t);
};

}

#endif  // !__plist_SimpleXMLParser_h
