#ifndef __NJI_DEBUG_H__
#define __NJI_DEBUG_H__


#if defined(__DEBUG__) && !defined(DEBUG)
#include <stdio.h>
#define DEBUG(...)                                      \
    do {                                                \
        fprintf(stderr, "%s:%d ", __FILE__, __LINE__);  \
        fprintf(stderr, __VA_ARGS__);                   \
    } while (0)
#else
#define DEBUG(fmt, ...)
#endif


#endif
