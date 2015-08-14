// Copyright 2013-present Facebook. All Rights Reserved.

#include <plist/SimpleXMLParser.h>

using plist::SimpleXMLParser;
using plist::Dictionary;

SimpleXMLParser::SimpleXMLParser() :
    BaseXMLParser(),
    _root          (nullptr),
    _current       (nullptr)
{
}

Dictionary *SimpleXMLParser::
parse(std::string const &path, error_function const &error)
{
    if (_root != nullptr)
        return nullptr;

    if (!BaseXMLParser::parse(path, error))
        return nullptr;

    return _root;
}

Dictionary *SimpleXMLParser::
parse(std::FILE *fp, error_function const &error)
{
    if (_root != nullptr)
        return nullptr;

    if (!BaseXMLParser::parse(fp, error))
        return nullptr;

    return _root;
}

static inline bool
IsNumber(std::string const &s)
{
    for (size_t n = 0; n < s.length(); n++) {
        if (!isdigit(s[n]))
            return false;
    }
    return true;
}

void SimpleXMLParser::
onBeginParse()
{
    _root = new Dictionary;
    _current = _root;
}

void SimpleXMLParser::
onEndParse(bool success)
{
    if (!success) {
        _root->release();
        _root = nullptr;
    }
    _current = nullptr;
}

void SimpleXMLParser::
onStartElement(std::string const &name, string_map const &attrs, size_t)
{
    Dictionary *dict = new Dictionary;

    for (auto I : attrs) {
        if (I.second == "YES") {
            dict->set(I.first, Boolean::New(true));
        } else if (I.second == "NO") {
            dict->set(I.first, Boolean::New(false));
        } else {
            dict->set(I.first, String::New(I.second));
        }
    }

    Object *old = const_cast <Object *> (_current->value(name));
    if (old != nullptr) {
        Array *array = CastTo <Array> (old);
        if (array == nullptr) {
            array = new Array;
            array->append(old);

            _current->remove(name, false);
            _current->set(name, array);
        }
        array->append(dict);
    } else {
        _current->set(name, dict);
    }

    _stack.push_back(_current);
    _current = dict;
}

void SimpleXMLParser::
onEndElement(std::string const &name, size_t)
{
    _current = _stack.back();
    _stack.pop_back();
}
