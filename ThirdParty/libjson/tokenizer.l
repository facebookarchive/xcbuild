/*
 * Copyright 2013-present Facebook. All rights reserved.
 * Copyright Nils Alexander Roemcke 2005.
 *
 * Use, modification, and distribution are subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

%{
#include "qstring.h"
%}

%option 8bit reentrant
%option bison-bridge
%option bison-locations
%option yylineno
%option noyywrap

%{
#include "parser.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
        yylloc->first_column = yycolumn; yylloc->last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;

struct string_buf {
    char *buffer;
    size_t capacity;
    size_t length;
};

static void
string_buf_init(struct string_buf *sbuf)
{
    memset(sbuf, 0, sizeof(struct string_buf));
}

static int
string_buf_grow(struct string_buf *sbuf, size_t length)
{
    char   *new_buffer;
    size_t  new_capacity;

    if (sbuf->length + length < sbuf->capacity)
        return 1;

    new_capacity = (sbuf->capacity + length + 255) & -256;
    new_buffer = (char *)realloc(sbuf->buffer, new_capacity);
    if (new_buffer == NULL)
        return 0;

    sbuf->buffer   = new_buffer;
    sbuf->capacity = new_capacity;

    return 1;
}

static int
string_buf_append(struct string_buf *sbuf, char const *text)
{
    size_t length = strlen(text);

    if (!string_buf_grow(sbuf, length + 1))
        return 0;

    memcpy(sbuf->buffer + sbuf->length, text, length + 1);
    sbuf->length += length;

    return 1;
}

static inline int is_a_number(char const *s)
{
    char const *p;
    for (p = s; *p != '\0'; p++) {
        if (!isdigit(*p))
            return 0;
    }
    return 1;
}
%}

%option yylineno

%x c_comment
%x cpp_comment
%x hex_data

IDENTIFIER    [_a-zA-Z\./\-][_0-9a-zA-Z\./\-]*
INTIDENTIFIER [0-9][_0-9a-zA-Z\./\-]+

INTEGER    "-"?(0|([1-9][0-9]*))
REAL       "-"?(0|([1-9][0-9]*))"."[0-9]+((e|E)("+"|"-")?[0-9]+)?
HEXDIGIT   [0-9a-fA-F]

STRING     "\""([^\n"\\]|("\\".))*"\""

BOPEN      "{"
BCLOSE     "}"
CBOPEN     "("
CBCLOSE    ")"
EQUAL      "="
SEMICOLON  ";"
COMMA      ","

YES        [Yy][Ee][Ss]
TRUE       [Tt][Rr][Uu][Ee]
NO         [Nn][Oo]
FALSE      [Ff][Aa][Ll][Ss][Ee]

%%

    struct string_buf hex_sbuf;

[ \n\r\t]+        /* eat up whitespace */

"//"                      BEGIN(cpp_comment);
<cpp_comment>[^\n]*
<cpp_comment>\n           BEGIN(INITIAL);

"/*"                      BEGIN(c_comment);
<c_comment>[^*\n]*
<c_comment>[^*\n]*\n      /* ++line_num; */
<c_comment>"*"+[^*/\n]*
<c_comment>"*"+[^*/\n]*\n /* ++line_num; */
<c_comment>"*"+"/"        BEGIN(INITIAL);

"<"                       { string_buf_init(&hex_sbuf);  BEGIN(hex_data); }
<hex_data>[ \t\n\r]*      /* */
<hex_data>{HEXDIGIT}*     { if (!string_buf_append(&hex_sbuf, yytext)) yyerror("out of memory"); }
<hex_data>">"             { BEGIN(INITIAL); yylval->v_string = hex_sbuf.buffer; string_buf_init(&hex_sbuf); return HEXDATA; }
<hex_data>.               { yyerror("expecting hexadecimal digit, found '%s'", yytext); }

{YES}            yylval->v_bool = -1; return JBOOLEAN;
{TRUE}           yylval->v_bool = -1; return JBOOLEAN;
{NO}             yylval->v_bool = 0; return JBOOLEAN;
{FALSE}          yylval->v_bool = 0; return JBOOLEAN;

{INTEGER}        yylval->v_int = atoi(yytext); return INTEGER;
{REAL}           yylval->v_double = atof(yytext); return REAL;

{IDENTIFIER}     yylval->v_string = strdup(yytext); return IDENTIFIER;
{INTIDENTIFIER}  {
                     if (is_a_number(yytext)) {
                         yylval->v_int = atoi(yytext); return INTEGER;
                     } else {
                         yylval->v_string = strdup(yytext); return IDENTIFIER;
                     }
                 }

{STRING}         yylval->v_string = _json_string_unquote(yytext); return STRING;

{BOPEN}          return BOPEN;
{BCLOSE}         return BCLOSE;
{CBOPEN}         return CBOPEN;
{CBCLOSE}        return CBCLOSE;
{EQUAL}          return EQUAL;
{SEMICOLON}      return SEMICOLON;
{COMMA}          return COMMA;

.                return yytext[0];

%%

/*
int main(int argc, char ** argv)
{
    yyscan_t yy;
    YYSTYPE lvalp;

    yylex_init(&yy);
    yylex(&lvalp, yy);
    yylex_destroy(yy);

    return 0;
}
*/
