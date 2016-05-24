/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <bom/bom.h>

#include <stdio.h>
#include <stdbool.h>

static bool
_bom_variable_dump(struct bom_context *context, const char *name, int data_index, void *ctx)
{
    printf("\t%s: index %x\n", name, data_index);
    return true;
}

static bool
_bom_index_dump(struct bom_context *context, uint32_t index, void *data, size_t data_len, void *ctx)
{
    printf("\t%x: data (%zx bytes)\n", index, data_len);
    return true;
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "error: missing argument\n");
        return 1;
    }

    struct bom_context *context;
    context = bom_alloc_load(bom_context_memory_file(argv[1], true, 0));
    if (context == NULL) {
        fprintf(stderr, "error: failed to load BOM\n");
        return 1;
    }

    printf("variables:\n");
    bom_variable_iterate(context, &_bom_variable_dump, NULL);

    printf("\n");

    printf("index:\n");
    bom_index_iterate(context, &_bom_index_dump, NULL);

    return 0;
}

