#include  "../include/ui.h"
#include  "../include/stack.h"
#include "../include/queue.h"


//TODO: add some logging system
// In the main function after InitWinfow() happens user is supposed to set
// the aproppriate veriables and call initUI()
// TODO: generalize this all
extern int windowWidth;
extern int windowHeight;

uiElement root = {0};
contextInfo rootContext = {0};
static stack context;
static stack elements;

void initUI(){
  root = (uiElement){
    .boundsNorm = (RectangleNorm){
      .x      = 0.0,
      .y      = 0.0,
      .height = 1.0,
      .width  = 1.0
    },
    .bounds = (Rectangle){
      .x      = 0,
      .y      = 0,
      .height = windowHeight,
      .width  = windowWidth
    }
    // everything else is already zeroed
  };
  contextInfo *contArray = malloc(10*sizeof(contextInfo));
  stackInit(&context, contArray, 10, sizeof(contextInfo));
  uiElement *elementArray = malloc(20*sizeof(uiElement));
  stackInit(&elements, elementArray, 20, sizeof(uiElement));
  push(&rootContext, &context);
  push(&root, &elements);
}
/* supposed usage:
 * openContext();
 * attach(1);
 * openContext();
 * attach(1.1);
 * attach(1.2);
 * openContext();
 * attach(1.2.1);
 * closeContext();
 * closeContext();
 * attach(2);
 * closeContext();
 */

void openContext(){
  contextInfo *lastContext = NULL;
  if(context.top > sizeof(contextInfo)){
    lastContext = peek(&context);
  }
  push(&(contextInfo){
        .childrenCount = 0, 
        .offset = !lastContext?0:lastContext->offset+lastContext->childrenCount
      }, &context);
}

void closeContext(){
  contextInfo *lastContext = pop(&context);
  if(lastContext->childrenCount){
    uiElement *children;
    children = malloc(lastContext->childrenCount * sizeof(uiElement));
    if(!children){
      printf("allocation failed");
      goto failed;
    }
    for(int i = 0; i<lastContext->childrenCount; i++){
      uiElement *child = pop(&elements);
      children[i] = *child;
      free(child);
    }
    uiElement *parent = peek(&elements);
    parent->numberOfChildren = lastContext->childrenCount;
    parent->children = children;
  }
failed:
  free(lastContext);
  puts("context closed");
}

void attach(uiElement element){
  contextInfo *lastContext = peek(&context);
  lastContext->childrenCount++;
  uiElement *parent = &((uiElement *)elements.s)[lastContext->offset];
  element.relativeAccum.x = parent->relativeAccum.x + parent->bounds.x;
  element.relativeAccum.y = parent->relativeAccum.y + parent->bounds.y;
  printf("parent acc %f %f\n", parent->relativeAccum.x, parent->relativeAccum.y);
  printf("child topleft %f %f\n", element.bounds.x, element.bounds.y);
  push(&element, &elements);
  puts("element attached");
}

void rootAttach(){
  uiElement *tmp = pop(&elements);
  root = *tmp;
}

// TODO: Collapse the BFS queue into a linear array, so that the BFS needs to be
// ran just once per UI scheme
void uiDrawUI(){
  queue uiDrawq;
  queueInit(&uiDrawq);
  uiElement current;
  enqueue(root, &uiDrawq);
again:
  current = dequeue(&uiDrawq);
  for(int i = 0; i<current.numberOfChildren; i++){
    enqueue(current.children[i], &uiDrawq);
  }
  switch(current.type){
    case UI_NONE:
      //printf("current topleft %f %f\n", current.bounds.x, current.bounds.y);
      //printf("current acc %f %f\n", current.relativeAccum.x, current.relativeAccum.y);
      DrawRectangleLines(current.bounds.x+current.relativeAccum.x, current.bounds.y+current.relativeAccum.y, current.bounds.width, current.bounds.height, RED);
      break;
    case UI_BUTTON:
      break;
  }
  if(uiDrawq.front != uiDrawq.back) goto again;
}




