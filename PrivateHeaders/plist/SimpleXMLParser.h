/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_SimpleXMLParser_h
#define __plist_SimpleXMLParser_h

#include <plist/BaseXMLParser.h>
#include <plist/Dictionary.h>

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
