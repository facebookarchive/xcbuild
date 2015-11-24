/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/ASCIIParser.h>
#include <plist/Objects.h>

using plist::Format::ASCIIParser;
using plist::Object;
using plist::String;
using plist::Data;
using plist::Array;
using plist::Dictionary;

ASCIIParser::
ASCIIParser() :
    _root(nullptr),
    _level(0),
    _state(ValueState::Init),
    _container(nullptr),
    _key(nullptr),
    _contextState(ContextState::Parsing)
{
}

ASCIIParser::
~ASCIIParser()
{
    while (!_containerStack.empty()) {
        Object *container = _containerStack.top();
        container->release();
        _containerStack.pop();
    }

    if (_container != NULL) {
        _container->release();
        _container = NULL;
    }

    while (!_keyStack.empty()) {
        String *key = _keyStack.top();
        key->release();
        _keyStack.pop();
    }

    if (_key != NULL) {
        _key->release();
        _key = NULL;
    }

    if (_root != NULL) {
        _root->release();
        _root = NULL;
    }
}

bool ASCIIParser::
isAborted() const
{
    return _contextState == ContextState::Aborted;
}

bool ASCIIParser::
isDone() const
{
    return _contextState == ContextState::Done;
}

int ASCIIParser::
getLevel() const
{
    return _level;
}

void ASCIIParser::
incrementLevel()
{
    _level++;
}

void ASCIIParser::
decrementLevel()
{
    if (_level != 0) {
        _level--;
    }
}

void ASCIIParser::
abort(std::string const &error)
{
    _error = error;
    _contextState = ContextState::Aborted;
}

bool ASCIIParser::
push(ValueState state, Object *container, String *key)
{
    if (isAborted()) {
        return false;
    }

    /* If valid state, push, otherwise just set the new state. */
    if (_state != ValueState::Init) {
        /* Push the old state */
        _containerStack.push(_container);
        _keyStack.push(_key);
        _stateStack.push(_state);
    }

    _state = state;

    if (container != NULL) {
        _container = container->copy();
    } else {
        _container = NULL;
    }

    if (key != NULL) {
        _key = (String *)key->copy();
    } else {
        _key = NULL;
    }

    return true;
}

bool ASCIIParser::
pop()
{
    Object *container;
    String *key;
    int     count = _stateStack.size();

    if (count == 0) {
        if (_state == ValueState::Init)
            return false; /* Underflow! */

        /* Reset current state. */
        if (_container != NULL) {
            _container->release();
            _container = NULL;
        }

        if (_key != NULL) {
            _key->release();
            _key = NULL;
        }

        _state = ValueState::Init;
        return true;
    }

    /* Pop state */
    _state = _stateStack.top();
    _stateStack.pop();

    if (_container != NULL) {
        _container->release();
    }
    _container = _containerStack.top();
    _containerStack.pop();

    if (_key != NULL) {
        _key->release();
    }
    _key = _keyStack.top();
    _keyStack.pop();

    return true;
}

/*
 * Generic container handling.
 */
bool ASCIIParser::
beginContainer(Object *object)
{
    ValueState state;

    if (object->type() == Array::Type()) {
        state = ValueState::Array;
    } else {
        state = ValueState::Dictionary;
    }

    if (!push(state, object, NULL)) {
        abort("Cannot push the current state.");
        return false;
    }

    return true;
}

bool ASCIIParser::
storeKeyValue(String *key, Object *value)
{
    if (_container == NULL) {
        if (_key != NULL) {
            abort("Storing key/value pair with no container.");
            return false;
        }

        /* Make current value the root. */
        if (_root != NULL) {
            abort("Double root.");
            return false;
        }

        _root = value->copy();
        return true;
    }

    if (key != NULL) {
        if (_container->type() != Dictionary::Type()) {
            abort("Storing key/value with no dictionary container.");
            return false;
        }

        ((Dictionary *)_container)->set(key->value(), value->copy());

        _state = ValueState::Dictionary;
    } else {
        if (_container->type() != Array::Type()) {
            abort("Storing value with no array container.");
            return false;
        }

        ((Array *)_container)->append(value->copy());

        _state = ValueState::Array;
    }

    return true;
}

bool ASCIIParser::
endContainer(bool isArray)
{
    String        *key;
    Object        *value;
    bool           success;

    /* Check state is consistant. */
    if (_state != (isArray ? ValueState::Array : ValueState::Dictionary)) {
        abort("Closing array/dictionary in wrong state.");
        return false;
    }

    if (_container == NULL) {
        abort("Closing non-opened container.");
        return false;
    }

    value = _container->copy();
    if (value == NULL) {
        abort("Couldn't copy container.");
        return false;
    }

    /* Is this the right type of container? */
    if (value->type() != (isArray ? Array::Type() : Dictionary::Type())) {
        abort("Closing wrong kind of container.");
        return false;
    }

    if (!pop()) {
        abort("Parser stack underflow.");
        return false;
    }

    /* Take ownership of the saved key. */
    key = _key;
    _key = NULL;

    success = storeKeyValue(key, value);

    if (key != NULL) {
        key->release();
    }
    value->release();

    return success;
}

bool ASCIIParser::
beginArray()
{
    Array   *array;

    array = Array::New();
    if (array == NULL) {
        abort("Cannot create an array.");
        return false;
    }

    bool success = beginContainer(array);
    return success;
}

bool ASCIIParser::
endArray()
{
    return endContainer(true);
}

bool ASCIIParser::
beginDictionary()
{
    Dictionary  *dict;

    dict = Dictionary::New();
    if (dict == NULL) {
        abort("Cannot create a dictionary.");
        return false;
    }

    bool success = beginContainer(dict);
    return success;
}

bool ASCIIParser::
endDictionary()
{
    return endContainer(false);
}

/*
 * Store the key of the current dictionary.
 */
bool ASCIIParser::
storeKey(String *key)
{
    String *copy;

    if (key == NULL) {
        copy = String::New();
    } else {
        copy = (String *)key->copy();
    }

    if (copy == NULL) {
        abort("Cannot create copy of the key.");
        return false;
    }

    if (_state != ValueState::Dictionary) {
        abort("Storing key in wrong state.");
        return false;
    }

    if (_key != NULL) {
        _key->release();
    }
    _key = copy;

    _state = ValueState::DictionaryValue;
    return true;
}

bool ASCIIParser::
storeValue(Object *value)
{
    bool success;

    success = storeKeyValue(_key, value);
    if (_key != NULL) {
        _key->release();
        _key = NULL;
    }

    if (_state == ValueState::DictionaryValue) {
        _state = ValueState::Dictionary;
    }

    return success;
}

bool ASCIIParser::
isDictionary() const
{
    return _state == ValueState::Dictionary;
}

bool ASCIIParser::
isArray() const
{
    return _state == ValueState::Array;
}

bool ASCIIParser::
finish()
{
    if (isDone()) {
        return true;
    }

    _contextState = ContextState::Done;
    return true;
}

#if 0
#define ASCIIDebug(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while (0)
#else
#define ASCIIDebug(...)
#endif

static inline uint8_t
hex_to_bin_digit(char ch)
{
    if (ch >= 'a' && ch <= 'f')
        return (ch - 'a') + 10;
    else if (ch >= 'A' && ch <= 'F')
        return (ch - 'A') + 10;
    else if (ch >= '0' && ch <= '9')
        return (ch - '0');
    else
        return 0;
}

static inline uint8_t
hex_to_bin(char const *ascii)
{
    return (hex_to_bin_digit(ascii[0]) << 4) | hex_to_bin_digit(ascii[1]);
}

bool ASCIIParser::
parse(ASCIIPListLexer *lexer)
{
    typedef enum _ASCIIParseState {
        kASCIIParsePList = 0,
        kASCIIParseKeyValSeparator,
        kASCIIParseEntrySeparator,

        kASCIIInvalid = -1
    } ASCIIParseState;

    int token;
    ASCIIParseState state = kASCIIParsePList;

    for (;;) {
        token = ASCIIPListLexerReadToken(lexer);
        if (token < 0) {
            if (token == kASCIIPListLexerEndOfFile && isDone()) {
                /* success */
                return true;
            } else if (token == kASCIIPListLexerEndOfFile) {
                abort("Encountered premature EOF");
                return false;
            } else if (token == kASCIIPListLexerInvalidToken) {
                abort("Encountered invalid token");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedLongComment) {
                abort("Encountered unterminated long comment");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedUnquotedString) {
                abort("Encountered unterminated unquoted string");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedQuotedString) {
                abort("Encountered unterminated quoted string");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedData) {
                abort("Encountered unterminated data");
                return false;
            } else {
                abort("Encountered unrecognized token error code");
                return false;
            }
        }

        /* Ignore comments */
        if (token == kASCIIPListLexerTokenInlineComment ||
            token == kASCIIPListLexerTokenLongComment)
            continue;

        switch (state) {
            case kASCIIParsePList:
                if (token != kASCIIPListLexerTokenUnquotedString &&
                    token != kASCIIPListLexerTokenQuotedString &&
                    token != kASCIIPListLexerTokenData &&
                    token != kASCIIPListLexerTokenNumber &&
                    token != kASCIIPListLexerTokenHexNumber &&
                    token != kASCIIPListLexerTokenBoolFalse &&
                    token != kASCIIPListLexerTokenBoolTrue &&
                    token != kASCIIPListLexerTokenDictionaryStart &&
                    token != kASCIIPListLexerTokenArrayStart &&
                    token != kASCIIPListLexerTokenDictionaryEnd &&
                    token != kASCIIPListLexerTokenArrayEnd) {
                    abort("Encountered unexpected token code");
                    return false;
                }

                if (isDone()) {
                    abort("Encountered token when finished.");
                    return false;
                }

                if (token != kASCIIPListLexerTokenDictionaryStart &&
                    token != kASCIIPListLexerTokenDictionaryEnd &&
                    token != kASCIIPListLexerTokenArrayStart &&
                    token != kASCIIPListLexerTokenArrayEnd) {
                    /* Read all non-container tokens */
                    bool topLevel     = getLevel() == 0;
                    bool isDictionary = this->isDictionary();

                    if (isDictionary || token == kASCIIPListLexerTokenUnquotedString || token == kASCIIPListLexerTokenQuotedString) {
                        if (isDictionary && token == kASCIIPListLexerTokenData) {
                            abort("Data cannot be dictionary key");
                            return false;
                        }

                        char *contents = ASCIIPListCopyUnquotedString(lexer, '?');
                        String *string = String::New(std::string(contents));
                        free(contents);

                        if (string == NULL) {
                            abort("OOM when copying string");
                            return false;
                        }

                        /* Container context */
                        if (isDictionary) {
                            ASCIIDebug("Storing string %s as key", string->value().c_str());
                            if (!storeKey(string)) {
                                string->release();
                                return false;
                            }
                        } else {
                            ASCIIDebug("Storing string %s", string->value().c_str());
                            if (!storeValue(string)) {
                                string->release();
                                return false;
                            }
                        }

                        string->release();
                    } else if (token == kASCIIPListLexerTokenNumber || token == kASCIIPListLexerTokenHexNumber) {
                        char *contents = ASCIIPListCopyUnquotedString(lexer, '?');
                        bool isReal = strchr(contents, '.') != NULL;

                        if (isReal) {
                            Real *real = Real::New(atof(contents));
                            free(contents);
                            if (real == NULL) {
                                abort("OOM when creating real");
                                return false;
                            }

                            ASCIIDebug("Storing real");
                            if (!storeValue(real)) {
                                real->release();
                                return false;
                            }

                            real->release();
                        } else {
                            Integer *integer = Integer::New(atol(contents));
                            free(contents);
                            if (integer == NULL) {
                                abort("OOM when creating integer");
                                return false;
                            }

                            ASCIIDebug("Storing integer");
                            if (!storeValue(integer)) {
                                integer->release();
                                return false;
                            }

                            integer->release();
                        }
                    } else if (token == kASCIIPListLexerTokenBoolTrue || token == kASCIIPListLexerTokenBoolFalse) {
                        bool value = (token == kASCIIPListLexerTokenBoolTrue);
                        Boolean *boolean = Boolean::New(value);
                        if (boolean == NULL) {
                            abort("OOM when creating boolean");
                            return false;
                        }

                        ASCIIDebug("Storing boolean");
                        if (!storeValue(boolean)) {
                            boolean->release();
                            return false;
                        }

                        boolean->release();
                    } else if (token == kASCIIPListLexerTokenData) {
                        char   *contents = ASCIIPListCopyData(lexer);
                        size_t  alength  = strlen(contents);
                        size_t  length   = alength / 2;
                        auto    bytes    = std::vector<uint8_t>(length, 0);

                        for (size_t n = 0; n < alength; n += 2) {
                            bytes[n >> 1] = hex_to_bin(contents + n);
                        }

                        Data *data = Data::New(bytes);
                        free(contents);

                        if (data == NULL) {
                            abort("OOM when copying data");
                            return false;
                        }

                        ASCIIDebug("Storing string as data");
                        if (!storeValue(data)) {
                            data->release();
                            return false;
                        }

                        data->release();
                    }

                    if (topLevel) {
                        if (!finish()) {
                            return false;
                        }
                        state = kASCIIParsePList;
                    } else {
                        if (isDictionary) {
                            state = kASCIIParseKeyValSeparator;
                        } else {
                            state = kASCIIParseEntrySeparator;
                        }
                    }
                } else if (token == kASCIIPListLexerTokenDictionaryStart) {
                    ASCIIDebug("Starting dictionary");
                    if (!beginDictionary()) {
                        return false;
                    }
                    incrementLevel();
                    state = kASCIIParsePList;
                } else if (token == kASCIIPListLexerTokenArrayStart) {
                    ASCIIDebug("Starting array");
                    if (!beginArray()) {
                        return false;
                    }
                    incrementLevel();
                    state = kASCIIParsePList;
                } else if (token == kASCIIPListLexerTokenDictionaryEnd) {
                    ASCIIDebug("Ending dictionary");
                    if (!endDictionary()) {
                        return false;
                    }
                    decrementLevel();
                    if (getLevel()) {
                        state = kASCIIParseEntrySeparator;
                    } else {
                        if (!finish()) {
                            return false;
                        }
                        state = kASCIIParsePList;
                    }
                } else if (token == kASCIIPListLexerTokenArrayEnd) {
                    ASCIIDebug("Ending array");
                    if (!endArray()) {
                        return false;
                    }
                    decrementLevel();
                    if (getLevel()) {
                        state = kASCIIParseEntrySeparator;
                    } else {
                        if (!finish()) {
                            return false;
                        }
                        state = kASCIIParsePList;
                    }
                }
                break;

            case kASCIIParseKeyValSeparator:
                if (token != kASCIIPListLexerTokenDictionaryKeyValSeparator) {
                    abort("Expected key-value separator; found something else");
                    return false;
                }
                ASCIIDebug("Found keyval separator");
                state = kASCIIParsePList;
                break;

            case kASCIIParseEntrySeparator:
                if (token != ';' && token != ',' &&
                    /*
                     * Arrays do not require a final separator. Dictionaries do.
                     */
                    token != kASCIIPListLexerTokenArrayEnd) {
                    abort("Expected entry separator or array end; found something else");
                    return false;
                }

                if (isDictionary() && token != ';') {
                    abort("Expected ';'");
                    return false;
                }

                if (isArray() &&
                    token != ',' &&
                    token != kASCIIPListLexerTokenArrayEnd) {
                    abort("Expected ',' or ')'");
                    return false;
                }

                if (token == kASCIIPListLexerTokenArrayEnd) {
                    if (isDictionary()) {
                        abort(NULL);
                        return false;
                    }
                    ASCIIDebug("Found array end");
                    if (!endArray()) {
                        return false;
                    }

                    decrementLevel();
                    if (getLevel()) {
                        state = kASCIIParseEntrySeparator;
                    } else {
                        if (!finish()) {
                            return false;
                        }
                        state = kASCIIParsePList;
                    }
                } else {
                    ASCIIDebug("Found entry separator");
                    state = kASCIIParsePList;
                }
                break;
            default:
                abort("Unexpected state");
                return false;
        }
    }
}

