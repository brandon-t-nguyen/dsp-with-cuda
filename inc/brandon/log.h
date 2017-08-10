#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

// error printing
#define eprintf(...) fprintf(stderr,"ERROR>> " __VA_ARGS__)

// toggleable compile time debug printing
#ifdef DEBUG
#define dprintf(...) fprintf(stderr,"DEBUG>> " __VA_ARGS__)
#else
#define dprintf(...)
#endif

#ifdef __cplusplus
}
#endif

#endif//__LOG_H__
