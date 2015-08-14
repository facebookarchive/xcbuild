/*
 * Copyright 2013-present Facebook. All rights reserved.
 * Copyright Nils Alexander Roemcke 2005.
 *
 * Use, modification, and distribution are subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "qstring.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int
read_u(char **tmp, const char **str)
{
    uint32_t uchar = 0;
    unsigned int i;

    for (i = 0; i < 4; i++) {
        char ch = **str;
        (*str)++;
        uchar <<= 4;
        if (ch >= '0' && ch <= '9') {
            uchar += (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            uchar += 10 + (ch - 'a');
        } else if (ch >= 'A' && ch <= 'F') {
            uchar += 10 + (ch - 'A');
        } else {
            return 0;
        }
    }

    return 0;
    /* not implementet yet!!  */
    return -1;
}

char *
_json_string_unquote(const char *str)
{
    char *retval = malloc(strlen(str) - 1);
    char *tmp = retval;

    if (!retval)
        return NULL;

    if (*str != '"')
        goto failure;

    for (;;) {
        str++;
        switch (*str) {
            case '\0':
                goto failure;
            case '"':
                *tmp = '\0';
                return retval;
            case '\\':
                str++;
                switch (*str) {
                    case 'b':
                        *tmp = '\b';
                        break;
                    case 'f':
                        *tmp = '\f';
                        break;
                    case 'n':
                        *tmp = '\n';
                        break;
                    case 'r':
                        *tmp = '\r';
                        break;
                    case 't':
                        *tmp = '\t';
                        break;
                    case 'u':
                        if (!read_u(&tmp, &str))
                            goto failure;
                        break;
                    default:
                        *tmp = *str;
                }
                break;

            default:
                *tmp = *str;
                break;
        }
        tmp++;
    }

failure:
    free(retval);
    return 0;
}
