#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

typedef struct{
  int capacity;
  int front;
  int back;
  
  void **q;
}queue;

int queueInit(queue *instance);
void enqueue(void *n, queue *instance);
void *dequeue(queue *instance);

