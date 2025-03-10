#include "../include/queue.h"

int queueInit(queue *instance){
  instance->capacity = 1024;
  instance->front = 0;
  instance->back = 0;
  instance->q = malloc(sizeof(int)*instance->capacity);
  if(!instance->q){
    puts("buy more ram");
    return -1;
  }
  return 0;
}

static void resize(queue *instance){
  uiElement *newq = malloc(instance->capacity*2*sizeof(uiElement));
  if(!newq){
    puts("failed queue resize");
  }
  for(int i = 0; i<instance->capacity; i++){
    newq[i] = instance->q[(instance->front+i)%instance->capacity];
  }
  free(instance->q);
  instance->back = instance->capacity;
  instance->front = 0;
  instance->capacity*=2;
  instance->q = newq;
}

void enqueue(uiElement n, queue *instance){
  instance->q[instance->back] = n;
  instance->back = (instance->back+1)%instance->capacity;
  if(instance->back == instance->front){
    resize(instance);
  }
}

uiElement dequeue(queue *instance){
  uiElement ret;
  ret = instance->q[instance->front];
  instance->front = (instance->front+1)%instance->capacity;
  return ret;
}

