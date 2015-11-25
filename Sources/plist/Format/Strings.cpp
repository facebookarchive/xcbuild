/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/Strings.h>
#include <plist/Format/ASCIIPListLexer.h>
#include <plist/Objects.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Format;
using plist::Format::Strings;
using plist::Object;
using plist::String;
using plist::Dictionary;

Strings::
Strings(Encoding encoding) :
    _encoding(encoding)
{
}

Type Strings::
Type()
{
    return Type::Strings;
}

template<>
std::unique_ptr<Strings> Format<Strings>::
Identify(std::vector<uint8_t> const &contents)
{
    /* Should be merged with ASCII parsing. */
    return nullptr;
}

/*
 * Strings format is
 *
 * "key" = "value";
 *
 * with C-style comments.
 */
static bool
StringsParserParse(ASCIIPListLexer *lexer, Dictionary *strings)
{
    int token;
    int state = 0;
    char *string;
    String *key = NULL;
    String *value = NULL;

    for (;;) {
        token = ASCIIPListLexerReadToken(lexer);
        if (token < 0) {
            if (key != NULL || value != NULL) {
                if (key != NULL) {
                    key->release();
                }
                if (value != NULL) {
                    value->release();
                }

                return false;
            } else if (token == kASCIIPListLexerEndOfFile) {
                return true;
            }
        }

        /* Ignore comments */
        if (token == kASCIIPListLexerTokenInlineComment ||
            token == kASCIIPListLexerTokenLongComment)
            continue;

        switch (state) {
            case 0: /* Initial state, expect a quoted string or a keyword. */
                if (token != kASCIIPListLexerTokenQuotedString &&
                    token != kASCIIPListLexerTokenUnquotedString)
                    return false;

                string = ASCIIPListCopyUnquotedString(lexer, '?');
                if (string == NULL)
                    return false;

                key = String::New(std::string(string));
                free(string);
                if (key == NULL)
                    return false;

                state = 1;
                break;

            case 1: /* State 1, expect a '=' */
                if (token != kASCIIPListLexerTokenDictionaryKeyValSeparator) {
                    key->release();
                    return false;
                }
                state = 2;
                break;

            case 2: /* State 2, expect another quoted string or a keyword. */
                if (token != kASCIIPListLexerTokenQuotedString &&
                    token != kASCIIPListLexerTokenUnquotedString) {
                    key->release();
                    return false;
                }

                string = ASCIIPListCopyUnquotedString(lexer, '?');
                if (string == NULL) {
                    key->release();
                    return false;
                }

                value = String::New(std::string(string));
                free(string);
                if (value == NULL) {
                    key->release();
                    return false;
                }

                state = 3;
                break;

            case 3: /* State 3, expect a semicolon */
                if (token != ';') {
                    value->release();
                    key->release();
                    return false;
                }

                strings->set(key->value(), value->copy());
                value->release();
                key->release();

                key = NULL;
                value = NULL;
                state = 0; /* Back to state 0 */
                break;

            default:
                return false;
        }
    }
}

template<>
std::pair<Object *, std::string> Format<Strings>::
Deserialize(std::vector<uint8_t> const &contents, Strings const &format)
{
    Dictionary         *root;
    std::string         error;
    ASCIIPListLexer     lexer;

    const std::vector<uint8_t> data = Encodings::Convert(contents, format.encoding(), Encoding::UTF8);

    root = Dictionary::New();
    if (root == NULL) {
        return std::make_pair(nullptr, "unable to create root");
    }

    ASCIIPListLexerInit(&lexer, reinterpret_cast<char const *>(data.data()), data.size(), kASCIIPListLexerStyleStrings);

    /* Parse contents. */
    if (!StringsParserParse(&lexer, root)) {
        root->release();
        root = NULL;

        error = "unable to parse strings";
    }

    return std::make_pair(root, error);
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format<Strings>::
Serialize(Object const *object, Strings const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

Strings Strings::
Create(Encoding encoding)
{
    return Strings(encoding);
}
