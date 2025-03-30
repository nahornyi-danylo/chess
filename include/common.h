#include <stdlib.h>
#include <stdio.h>
// A header which will hold common things, that are so small, they don't deserve a separate file

// LOG_PREFIX has to be defined before the include
#ifndef LOG_PREFIX
#define LOG_PREFIX " "
#endif

#define LOG(fmt, ...) printf("[" LOG_PREFIX "] " fmt, ##__VA_ARGS__)

#ifdef DEBUG
#define DEBUG_LOG(fmt, ...) printf("[DEBUG " LOG_PREFIX "] " fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...)
#endif

#ifndef COMMON_IMPLEMENTATION
extern int allocCounter;
extern int freeCounter;
#endif


void *mallocWrap(int size, const char *file, int line);
void freeWrap(void *ptr, const char *file, int line);
#define bestow(size) mallocWrap(size, __FILE__, __LINE__)
#define relinquish(ptr) freeWrap(ptr, __FILE__, __LINE__)

