#define COMMON_IMPLEMENTATION
#define LOG_PREFIX "common"
#include "../include/common.h"

int allocCounter = 0;
int freeCounter = 0;

void *mallocWrap(int size, const char *file, int line){
  void *result = malloc(size);
  if(!result){
    LOG("Allocation failed at %s, %d\n", file, line);
    exit(1);
  }
  allocCounter++;
  DEBUG_LOG("allocated %p at %d in %s\n", result, line, file);

  return result;
}

void freeWrap(void *ptr, const char *file, int line){
  freeCounter++;
  DEBUG_LOG("freed %p at %d in %s\n", ptr, line, file);
  free(ptr);
}
