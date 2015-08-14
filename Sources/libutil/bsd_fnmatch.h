/* Copyright 2013-present Facebook. All Rights Reserved. */
#ifndef __bsd_fnmatch_h
#define __bsd_fnmatch_h

#include <sys/types.h>

#define BSD_FNM_NOESCAPE    (1 << 0)
#define BSD_FNM_PATHNAME    (1 << 1)
#define BSD_FNM_PERIOD      (1 << 2)
#define BSD_FNM_ESCAPE      (1 << 3)
#define BSD_FNM_LEADING_DIR (1 << 4)
#define BSD_FNM_CASEFOLD    (1 << 5)

#define BSD_FNM_NOMATCH     (-1)

#ifdef __cplusplus
extern "C" {
#endif

int bsd_fnmatch(const char *pattern, const char *string, int flags);

#ifdef __cplusplus
}
#endif

#endif  /* !__bsd_fnmatch_h */
