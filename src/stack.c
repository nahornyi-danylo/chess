#include "../include/stack.h"

static void resize(stack *instance){
  instance->capacity*=2;
  instance->s = realloc(instance->s, sizeof(int)*instance->capacity);
  if(!instance->s){
    puts("stack reallocation failed");
  }
}

void push(void *n, stack *instance){
  for(int i = 0; i<instance->elementSize; i++){
    ((unsigned char *)instance->s)[instance->top+i] = ((unsigned char *)n)[i];
  }
  instance->top+=instance->elementSize;
  if(instance->top == instance->capacity)resize(instance);
}

void *pop(stack *instance){
  instance->top-=instance->elementSize;
  void *new = malloc(instance->elementSize);
  if(!new){
    puts("allocation on pop failed");
    return NULL;
  }
  for(int i = 0; i<instance->elementSize; i++){
    ((unsigned char *)new)[i] = ((unsigned char *)instance->s)[instance->top+i];
  }
  return new;
}

void *peek(stack *instance){
  unsigned char *res = instance->s;
  return (void *)&res[instance->top-instance->elementSize];
}

int stackInit(stack *instance, void *array, int arraylen, int elementSize){
  instance->capacity = elementSize * arraylen;
  instance->top = 0;
  instance->elementSize = elementSize;
  instance->s = array;
  if(!instance->s){
    puts("Passed a null pointer");
    return -1;
  }
  return 0;
}

