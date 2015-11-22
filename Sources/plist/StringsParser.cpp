/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/StringsParser.h>
#include <plist/Format/ASCIIPListLexer.h>
#include <plist/Format/ASCIIPListParser.h>

#include <plist/Objects.h>

using plist::StringsParser;
using plist::Object;
using plist::String;
using plist::Integer;
using plist::Real;
using plist::Boolean;
using plist::Data;
using plist::Array;
using plist::Dictionary;

StringsParser::StringsParser()
{
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

Object *StringsParser::
parse(std::string const &path, error_function const &error)
{
    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if (fp == nullptr)
        return nullptr;

    Object *object = parse(fp, error);

    std::fclose(fp);
    return object;
}

Object *StringsParser::
parse(std::FILE *fp, error_function const &error)
{
    Dictionary         *root;
    bool                success;
    ASCIIPListLexer     lexer;

    if (fseek(fp, 0, SEEK_END) != 0) {
        return nullptr;
    }

    long fsize = ftell(fp);
    if (fsize == -1) {
        return nullptr;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        return nullptr;
    }

    char *data = (char *)malloc(fsize + 1);
    if (data == NULL) {
        return nullptr;
    }

    if (fread(data, fsize, 1, fp) != 1) {
        return nullptr;
    }

    root = Dictionary::New();
    if (root == NULL) {
        return nullptr;
    }

    ASCIIPListLexerInit(&lexer, data, fsize, kASCIIPListLexerStyleStrings);

    /* Parse contents. */
    success = StringsParserParse(&lexer, root);
    if (!success) {
        error(0, 0, "Unable to parse.");

        root->release();
        root = NULL;
    }

    free(data);
    return root;
}

