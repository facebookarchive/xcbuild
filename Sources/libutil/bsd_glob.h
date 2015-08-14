/* Copyright 2013-present Facebook. All Rights Reserved. */
#ifndef __bsd_glob_h
#define __bsd_glob_h

#include <sys/types.h>

#define BSD_GLOB_QUOTE      (1 << 0)
#define BSD_GLOB_MAGCHAR    (1 << 1)
#define BSD_GLOB_NOMAGIC    (1 << 2)
#define BSD_GLOB_ALTDIRFUNC (1 << 3)
#define BSD_GLOB_TILDE      (1 << 4)
#define BSD_GLOB_BRACE      (1 << 5)
#define BSD_GLOB_APPEND     (1 << 6)
#define BSD_GLOB_DOOFFS     (1 << 7)
#define BSD_GLOB_NOSORT     (1 << 8)
#define BSD_GLOB_NOESCAPE   (1 << 9)
#define BSD_GLOB_LIMIT      (1 << 10)
#define BSD_GLOB_MARK       (1 << 11)
#define BSD_GLOB_NOCHECK    (1 << 12)
#define BSD_GLOB_KEEPSTAT   (1 << 13)
#define BSD_GLOB_ERR        (1 << 14)
#define BSD_GLOB_CASEFOLD   (1 << 15) /* extension */

#define BSD_GLOB_NOMATCH    (-1)
#define BSD_GLOB_NOSPACE    (-2)
#define BSD_GLOB_ABORTED    (-3)

typedef struct bsd_glob {
    size_t         gl_pathc;
    size_t         gl_matchc;
    size_t         gl_offs;
    unsigned       gl_flags;
    char         **gl_pathv;
    struct stat  **gl_statv;
    int          (*gl_errfunc)(const char *, int);
    void        *(*gl_opendir)(const char *);
    void         (*gl_closedir)(void *);
    void        *(*gl_readdir)(void *);
    int          (*gl_lstat)(const char *, void *);
    int          (*gl_stat)(const char *, void *);
} bsd_glob_t;

#ifdef __cplusplus
extern "C" {
#endif

int bsd_glob(const char *pattern, int flags, int (*errfunc)(const char *, int),
        bsd_glob_t *pglob);
void bsd_globfree(bsd_glob_t *pglob);

#ifdef __cplusplus
}
#endif

#endif  /* !__bsd_glob_h */
