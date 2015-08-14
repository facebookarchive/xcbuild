// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __plist_XMLParser_h
#define __plist_XMLParser_h

#include <plist/BaseXMLParser.h>
#include <plist/Objects.h>

namespace plist {

class XMLParser : public BaseXMLParser {
private:
    struct Key {
        std::string value;
        bool        valid;
        bool        active;
    };

    struct State {
        typedef std::vector <State> vector;

        Object *current;
        Key     key;
    };

private:
    Object        *_root;
    State::vector  _stack;
    State          _state;
    std::string    _cdata;

public:
    XMLParser();

public:
    Object *parse(std::string const &path, error_function const &error);
    Object *parse(std::FILE *fp, error_function const &error);

private:
    virtual void onBeginParse();
    virtual void onEndParse(bool success);

private:
    void onStartElement(std::string const &name, string_map const &attrs, size_t depth);
    void onEndElement(std::string const &name, size_t depth);
    void onCharacterData(std::string const &cdata, size_t depth);

private:
    void push(Object *object);
    void pop();

private:
    inline bool inArray() const;
    inline bool inDictionary() const;
    inline bool inContainer() const;
    inline bool isExpectingKey() const;
    inline bool isExpectingCDATA() const;

private:
    bool beginObject(std::string const &name);
    bool beginArray();
    bool beginDictionary();
    bool beginString();
    bool beginInteger();
    bool beginReal();
    bool beginBoolean(bool value);
    bool beginNull();
    bool beginData();
    bool beginDate();
    bool beginKey();

private:
    bool endObject(std::string const &name);
    bool endArray();
    bool endDictionary();
    bool endString();
    bool endInteger();
    bool endReal();
    bool endBoolean();
    bool endNull();
    bool endData();
    bool endDate();
    bool endKey();
};

}

#endif  // !__plist_XMLParser_h
