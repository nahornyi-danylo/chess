#include <stdio.h>
#include <stdlib.h>

typedef struct{
  int capacity;
  int top;
  int elementSize;
  
  void **s;
}stack;

int stackInit(stack *instance, void *array, int arraylen);
void *pop(stack *instance);
void *peek(stack *instance);
void push(void *n, stack *instance);
