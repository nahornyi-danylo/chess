#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

typedef struct{
  int capacity;
  int front;
  int back;
  
  uiElement *q;
}queue;

int queueInit(queue *instance);
void enqueue(uiElement n, queue *instance);
uiElement dequeue(queue *instance);

