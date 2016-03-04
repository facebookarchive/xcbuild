/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <bom/bom.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

static void
_bom_context_memory_realloc(struct bom_context_memory *memory, size_t size)
{
    memory->size = size;
    memory->data = realloc(memory->data, memory->size);
}

static void
_bom_context_memory_free(struct bom_context_memory *memory)
{
    free(memory->data);
}

struct bom_context_memory
bom_context_memory(void)
{
    return (struct bom_context_memory){
        .data = malloc(0),
        .size = 0,
        .resize = _bom_context_memory_realloc,
        .free = _bom_context_memory_free,
        .ctx = NULL,
    };
}

struct _bom_context_memory_mmap_context {
    int fd;
    bool writeable;
};

static void
_bom_context_memory_mremap(struct bom_context_memory *memory, size_t size)
{
    struct _bom_context_memory_mmap_context *context = memory->ctx;

    munmap(memory->data, memory->size);
    if (size > memory->size) {
        int ret = ftruncate(context->fd, size);
        assert(ret == 0);
        (void)ret;
    }

    memory->size = size;

    int prot = context->writeable ? PROT_READ | PROT_WRITE : PROT_READ;
    memory->data = mmap(NULL, size, prot, MAP_SHARED, context->fd, 0);
    assert((int)memory->data != -1);
}

static void
_bom_context_memory_munmap(struct bom_context_memory *memory)
{
    struct _bom_context_memory_mmap_context *context = memory->ctx;

    munmap(memory->data, memory->size);
    close(context->fd);
    free(context);
}

struct bom_context_memory
bom_context_memory_file(const char *fn, bool writeable)
{
    int fd = open(fn, (writeable ? O_RDWR : O_RDONLY) | O_CREAT, 0755);

    struct stat st;
    fstat(fd, &st);

    struct _bom_context_memory_mmap_context *context = malloc(sizeof(*context));
    context->fd = fd;
    context->writeable = writeable;

    int prot = context->writeable ? PROT_READ | PROT_WRITE : PROT_READ;
    void *data = mmap(NULL, st.st_size, prot, MAP_SHARED, context->fd, 0);

    return (struct bom_context_memory) {
        .data = data,
        .size = st.st_size,
        .resize = _bom_context_memory_mremap,
        .free = _bom_context_memory_munmap,
        .ctx = context,
    };
}



