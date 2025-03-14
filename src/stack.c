#include "../include/stack.h"

#define LOG_PREFIX "[stack] "
#include "../include/common.h"

static void resize(stack *instance){
  instance->capacity*=2;
  instance->s = realloc(instance->s, sizeof(void *)*instance->capacity);
  if(!instance->s){
    LOG("stack reallocation failed");
  }
}

void push(void *n, stack *instance){
  instance->s[instance->top] = n;
  instance->top++;
  LOG("pushed %p\n", n);
  if(instance->top == instance->capacity) resize(instance);
}

void *pop(stack *instance){
  instance->top--;
  LOG("popped %p\n", instance->s[instance->top]);
  return instance->s[instance->top];
}

void *peek(stack *instance){
  LOG("peeked %p\n", instance->s[instance->top-1]);
  return instance->s[instance->top-1];
}

int stackInit(stack *instance, void *array, int arraylen){
  instance->capacity = arraylen;
  instance->top = 0;
  instance->s = array;
  if(!instance->s){
    puts("Passed a null pointer");
    return -1;
  }
  return 0;
}

