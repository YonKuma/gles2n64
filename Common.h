

#ifndef __COMMON_H__
#define __COMMON_H__

#define LOG_LEVEL 10

#define LOG_NONE	0
#define LOG_ERROR   1
#define LOG_MINIMAL	2
#define LOG_WARNING 3
#define LOG_VERBOSE 4

#if LOG_LEVEL>0

#define LOG(A, ...) \
    if (A <= LOG_LEVEL) \
    { \
        if (A == LOG_WARNING) printf("(WW) "); \
        if (A == LOG_ERROR) printf("(EE) "); \
        printf(__VA_ARGS__); \
        fflush(stdout); \
    }

#else

#define LOG(A, ...)

#endif

#endif
