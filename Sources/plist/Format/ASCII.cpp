/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/ASCII.h>
#include <plist/Format/ASCIIPListLexer.h>
#include <plist/Format/ASCIIPListParser.h>
#include <plist/Format/ASCIIWriter.h>
#include <plist/Objects.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Format;
using plist::Format::ASCII;
using plist::Object;
using plist::String;
using plist::Integer;
using plist::Real;
using plist::Boolean;
using plist::Data;
using plist::Array;
using plist::Dictionary;

ASCII::
ASCII(Encoding encoding) :
    _encoding(encoding)
{
}

Type ASCII::
Type()
{
    return Type::ASCII;
}

template<>
std::unique_ptr<ASCII> Format<ASCII>::
Identify(std::vector<uint8_t> const &contents)
{
    /*
     * Identification of ASCII is as follows:
     *
     * If BOM is present, it's unicode encoded, otherwise MacRoman.
     * Any C / * ... * / and C++ // style comments are skipped.
     * The start of any data (disambiguated from XML), dictionary, array,
     * quoted string, or unquoted string marks the start (checks for no '=' after
     * the string to make sure this isn't strings format).
     *
     * TODO: Encoding may also be specified in the leading inline comment. Handle this?
     */

    enum State {
        kStateBegin,
        kStateComment,
        kStateInlineComment,
        kStateIdentifier,
        kStateQuoting,
        kStateEqual,
    };

    uint8_t last = '\0';
    enum State state = kStateBegin, pstate = state;

    for (std::vector<uint8_t>::const_iterator bp = contents.begin(); bp != contents.end(); ++bp) {
        /* Conceal zeroes for UTF-16/32 encodings. */
        if (*bp == 0 || (state != kStateComment &&
                         state != kStateInlineComment &&
                         state != kStateIdentifier &&
                         isspace(*bp))) {
            bp++;
            continue;
        }

        if (state == kStateComment) {
            switch (*bp) {
                case '/':
                    if (last == '*') {
                        state = pstate;
                    }
                    last = 0;
                    break;
                case '*':
                    last = *bp;
                    break;
                default:
                    break;
            }
            bp++;
            continue;
        } else if (state == kStateInlineComment) {
            if (*bp == '\n') {
                state = pstate;
                last = 0;
            }

            bp++;
            continue;
        } else {
            switch (*bp) {
                case '/':
                    if (last == '/') {
                        pstate = state;
                        state = kStateInlineComment;
                        last = 0;
                        bp++;
                    } else if (last != 0) {
                        return nullptr;
                    } else {
                        last = *bp++;
                    }
                    continue;

                case '*':
                    if (last == '/') {
                        pstate = state;
                        state = kStateComment;
                    }
                    last = 0;
                    bp++;
                    continue;

                default:
                    break;
            }
        }

        if (state == kStateBegin) {
            switch (*bp) {
                    /*
                     * We conceal some BOM chars for UTF encodings
                     * in the first four bytes.
                     */
                case 0xfe: /* UTF-16/32 */
                case 0xff:
                case 0xef: /* UTF-8 */
                case 0xbb:
                case 0xbf:
                    if (bp - contents.begin() < 4) {
                        bp++;
                        continue;
                    } else {
                        return nullptr;
                    }
                case '/': /* Comments */
                case '*':
                    break;
                case '\"':
                case '(': {
                    Encoding encoding = Encodings::Detect(contents);
                    return std::make_unique<ASCII>(ASCII::Create(encoding));
                }
                case '{':
                    state = kStateIdentifier;
                    bp++;
                    break;
                case '<':
                    if (bp[1] != '?' && bp[1] != '!') {
                        Encoding encoding = Encodings::Detect(contents);
                        return std::make_unique<ASCII>(ASCII::Create(encoding));
                    }
                    return nullptr;
                default:
                    if (isalnum(*bp) || *bp == '_' || *bp == '.' || *bp == '$') {
                        Encoding encoding = Encodings::Detect(contents);
                        return std::make_unique<ASCII>(ASCII::Create(encoding));
                    }
                    return nullptr;
            }
        } else if (state == kStateIdentifier) {
            if (isspace(*bp) && last == 0) {
                /* Haven't started the identifier yet */
            } else if (*bp == '"') {
                state = kStateQuoting;
            } else if (isspace(*bp) && last) {
                state = kStateEqual;
                last = 0;
            } else {
                last = *bp;
            }
            
            bp++;
        } else if (state == kStateQuoting) {
            if (*bp == '"' && last != '\\') {
                state = kStateEqual;
                last = 0;
            } else {
                last = *bp;
            }
            bp++;
        } else if (state == kStateEqual) {
            if (*bp != '=') {
                /* Probably JSON */
                return nullptr;
            }
            
            Encoding encoding = Encodings::Detect(contents);
            return std::make_unique<ASCII>(ASCII::Create(encoding));
        } else {
            return nullptr;
        }

        last = *bp;
    }

    return nullptr;
}

typedef enum _ASCIIParseState {
    kASCIIParsePList = 0,
    kASCIIParseKeyValSeparator,
    kASCIIParseEntrySeparator,

    kASCIIInvalid = -1
} ASCIIParseState;

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

static bool
ASCIIParserParse(ASCIIPListParserContext *context, ASCIIPListLexer *lexer)
{
    int token;
    ASCIIParseState state = kASCIIParsePList;

    for (;;) {
        token = ASCIIPListLexerReadToken(lexer);
        if (token < 0) {
            if (token == kASCIIPListLexerEndOfFile && ASCIIPListParserIsDone(context)) {
                /* success */
                return true;
            } else if (token == kASCIIPListLexerEndOfFile) {
                ASCIIPListParserAbort(context, "Encountered premature EOF");
                return false;
            } else if (token == kASCIIPListLexerInvalidToken) {
                ASCIIPListParserAbort(context, "Encountered invalid token");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedLongComment) {
                ASCIIPListParserAbort(context, "Encountered unterminated long comment");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedUnquotedString) {
                ASCIIPListParserAbort(context, "Encountered unterminated unquoted string");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedQuotedString) {
                ASCIIPListParserAbort(context, "Encountered unterminated quoted string");
                return false;
            } else if (token == kASCIIPListLexerUnterminatedData) {
                ASCIIPListParserAbort(context, "Encountered unterminated data");
                return false;
            } else {
                ASCIIPListParserAbort(context, "Encountered unrecognized token error code");
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
                    ASCIIPListParserAbort(context, "Encountered unexpected token code");
                    return false;
                }

                if (ASCIIPListParserIsDone(context)) {
                    ASCIIPListParserAbort(context, "Encountered token when finished.");
                    return false;
                }

                if (token != kASCIIPListLexerTokenDictionaryStart &&
                    token != kASCIIPListLexerTokenDictionaryEnd &&
                    token != kASCIIPListLexerTokenArrayStart &&
                    token != kASCIIPListLexerTokenArrayEnd) {
                    /* Read all non-container tokens */
                    bool topLevel     = ASCIIPListParserGetLevel(context) == 0;
                    bool isDictionary = ASCIIPListParserIsDictionary(context);

                    if (isDictionary || token == kASCIIPListLexerTokenUnquotedString || token == kASCIIPListLexerTokenQuotedString) {
                        if (isDictionary && token == kASCIIPListLexerTokenData) {
                            ASCIIPListParserAbort(context, "Data cannot be dictionary key");
                            return false;
                        }

                        char *contents = ASCIIPListCopyUnquotedString(lexer, '?');
                        String *string = String::New(std::string(contents));
                        free(contents);

                        if (string == NULL) {
                            ASCIIPListParserAbort(context, "OOM when copying string");
                            return false;
                        }

                        /* Container context */
                        if (isDictionary) {
                            ASCIIDebug("Storing string %s as key", string->value().c_str());
                            if (!ASCIIPListParserStoreKey(context, string)) {
                                string->release();
                                return false;
                            }
                        } else {
                            ASCIIDebug("Storing string %s", string->value().c_str());
                            if (!ASCIIPListParserStoreValue(context, string)) {
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
                                ASCIIPListParserAbort(context, "OOM when creating real");
                                return false;
                            }

                            ASCIIDebug("Storing real");
                            if (!ASCIIPListParserStoreValue(context, real)) {
                                real->release();
                                return false;
                            }

                            real->release();
                        } else {
                            Integer *integer = Integer::New(atol(contents));
                            free(contents);
                            if (integer == NULL) {
                                ASCIIPListParserAbort(context, "OOM when creating integer");
                                return false;
                            }

                            ASCIIDebug("Storing integer");
                            if (!ASCIIPListParserStoreValue(context, integer)) {
                                integer->release();
                                return false;
                            }

                            integer->release();
                        }
                    } else if (token == kASCIIPListLexerTokenBoolTrue || token == kASCIIPListLexerTokenBoolFalse) {
                        bool value = (token == kASCIIPListLexerTokenBoolTrue);
                        Boolean *boolean = Boolean::New(value);
                        if (boolean == NULL) {
                            ASCIIPListParserAbort(context, "OOM when creating boolean");
                            return false;
                        }

                        ASCIIDebug("Storing boolean");
                        if (!ASCIIPListParserStoreValue(context, boolean)) {
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
                            ASCIIPListParserAbort(context, "OOM when copying data");
                            return false;
                        }

                        ASCIIDebug("Storing string as data");
                        if (!ASCIIPListParserStoreValue(context, data)) {
                            data->release();
                            return false;
                        }

                        data->release();
                    }

                    if (topLevel) {
                        if (!ASCIIPListParserFinish(context)) {
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
                    if (!ASCIIPListParserDictionaryBegin(context)) {
                        return false;
                    }
                    ASCIIPListParserIncrementLevel(context);
                    state = kASCIIParsePList;
                } else if (token == kASCIIPListLexerTokenArrayStart) {
                    ASCIIDebug("Starting array");
                    if (!ASCIIPListParserArrayBegin(context)) {
                        return false;
                    }
                    ASCIIPListParserIncrementLevel(context);
                    state = kASCIIParsePList;
                } else if (token == kASCIIPListLexerTokenDictionaryEnd) {
                    ASCIIDebug("Ending dictionary");
                    if (!ASCIIPListParserDictionaryEnd(context)) {
                        return false;
                    }
                    ASCIIPListParserDecrementLevel(context);
                    if (ASCIIPListParserGetLevel(context)) {
                        state = kASCIIParseEntrySeparator;
                    } else {
                        if (!ASCIIPListParserFinish(context)) {
                            return false;
                        }
                        state = kASCIIParsePList;
                    }
                } else if (token == kASCIIPListLexerTokenArrayEnd) {
                    ASCIIDebug("Ending array");
                    if (!ASCIIPListParserArrayEnd(context)) {
                        return false;
                    }
                    ASCIIPListParserDecrementLevel(context);
                    if (ASCIIPListParserGetLevel(context)) {
                        state = kASCIIParseEntrySeparator;
                    } else {
                        if (!ASCIIPListParserFinish(context)) {
                            return false;
                        }
                        state = kASCIIParsePList;
                    }
                }
                break;

            case kASCIIParseKeyValSeparator:
                if (token != kASCIIPListLexerTokenDictionaryKeyValSeparator) {
                    ASCIIPListParserAbort(context, "Expected key-value separator; found something else");
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
                    ASCIIPListParserAbort(context, "Expected entry separator or array end; found something else");
                    return false;
                }

                if (ASCIIPListParserIsDictionary(context) && token != ';') {
                    ASCIIPListParserAbort(context, "Expected ';'");
                    return false;
                }

                if (ASCIIPListParserIsArray(context) &&
                    token != ',' &&
                    token != kASCIIPListLexerTokenArrayEnd) {
                    ASCIIPListParserAbort(context, "Expected ',' or ')'");
                    return false;
                }

                if (token == kASCIIPListLexerTokenArrayEnd) {
                    if (ASCIIPListParserIsDictionary(context)) {
                        ASCIIPListParserAbort(context, NULL);
                        return false;
                    }
                    ASCIIDebug("Found array end");
                    if (!ASCIIPListParserArrayEnd(context)) {
                        return false;
                    }

                    ASCIIPListParserDecrementLevel(context);
                    if (ASCIIPListParserGetLevel(context)) {
                        state = kASCIIParseEntrySeparator;
                    } else {
                        if (!ASCIIPListParserFinish(context)) {
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
                ASCIIPListParserAbort(context, "Unexpected state");
                return false;
        }
    }
}

template<>
std::pair<Object *, std::string> Format<ASCII>::
Deserialize(std::vector<uint8_t> const &contents, ASCII const &format)
{
    Object                  *root = nullptr;
    std::string              error;

    ASCIIPListParserContext  context;
    ASCIIPListLexer          lexer;

    const std::vector<uint8_t> data = Encodings::Convert(contents, format.encoding(), Encoding::UTF8);

    ASCIIPListLexerInit(&lexer, reinterpret_cast<char const *>(data.data()), data.size(), kASCIIPListLexerStyleASCII);
    ASCIIPListParserContextInit(&context);

    /* Parse contents. */
    if (ASCIIParserParse(&context, &lexer)) {
        root = ASCIIPListParserCopyRoot(&context);
    } else {
        error = context.error;
    }

    ASCIIPListParserContextFree(&context);

    return std::make_pair(root, error);
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format<ASCII>::
Serialize(Object *object, ASCII const &format)
{
    if (object == nullptr) {
        return std::make_pair(nullptr, "object was null");
    }

    ASCIIWriter writer = ASCIIWriter(object);
    if (!writer.write()) {
        return std::make_pair(nullptr, "serialization failed");
    }

    const std::vector<uint8_t> data = Encodings::Convert(writer.contents(), Encoding::UTF8, format.encoding());

    return std::make_pair(std::make_unique<std::vector<uint8_t>>(data), std::string());
}

ASCII ASCII::
Create(Encoding encoding)
{
    return ASCII(encoding);
}
