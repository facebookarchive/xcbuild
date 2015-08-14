/* Copyright 2013-present Facebook. All rights reserved. */

#include "jsoncb.h"

int
main(int argc, char **argv)
{
    json_fparse(stdin, NULL, NULL, NULL, NULL);

    return 0;
}
