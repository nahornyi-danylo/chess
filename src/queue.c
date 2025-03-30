#include "../include/queue.h"
#include "../include/common.h"

int queueInit(queue *instance){
  instance->capacity = 1024;
  instance->front = 0;
  instance->back = 0;
  instance->q = bestow(sizeof(void *)*instance->capacity);

  return 0;
}

static void resize(queue *instance){
  void **newq = bestow(instance->capacity*2*sizeof(void *));

  for(int i = 0; i<instance->capacity; i++){
    newq[i] = instance->q[(instance->front+i)%instance->capacity];
  }
  relinquish(instance->q);
  instance->back = instance->capacity;
  instance->front = 0;
  instance->capacity*=2;
  instance->q = newq;
}

void enqueue(void *n, queue *instance){
  instance->q[instance->back] = n;
  instance->back = (instance->back+1)%instance->capacity;
  if(instance->back == instance->front){
    resize(instance);
  }
}

void *dequeue(queue *instance){
  void *ret;
  ret = instance->q[instance->front];
  instance->front = (instance->front+1)%instance->capacity;
  return ret;
}

