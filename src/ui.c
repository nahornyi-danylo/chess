#include  "../include/ui.h"
#include  "../include/stack.h"
#include "../include/queue.h"

#define LOG_PREFIX "[UI] "
#include "../include/common.h"


//TODO: add some logging system
// In the main function after InitWinfow() happens user is supposed to set
// the aproppriate veriables and call initUI()
// TODO: generalize this all
static int windowWidth;
static int windowHeight;

uiElement *root;
contextInfo *rootContext;
static stack context;
static stack elements;

void initUI(uiElement rootEl, int width, int height){
  root = malloc(sizeof(uiElement));
  LOG("init at %p\n", root);
  rootContext = malloc(sizeof(contextInfo));
  *root = rootEl;
  *rootContext = (contextInfo){0};
  windowWidth = width;
  windowHeight = height;
  contextInfo *contArray = malloc(10*sizeof(contextInfo));
  stackInit(&context, contArray, 10);
  uiElement *elementArray = malloc(20*sizeof(uiElement));
  stackInit(&elements, elementArray, 20);
  push(rootContext, &context);
  push(root, &elements);
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
  if(context.top > 0){
    lastContext = peek(&context);
  }
  contextInfo *newContext = malloc(sizeof(contextInfo));
  if(!newContext){
    LOG("!CRITICAL context allocation failed\n");
    exit(1);
  }
  newContext->childrenCount = 0;
  newContext->offset = !lastContext?0:lastContext->offset+lastContext->childrenCount;
  push(newContext, &context);
}

void closeContext(){
  if(context.top > 0){
    contextInfo *lastContext = pop(&context);
    LOG("offset %d\n", lastContext->offset);


    if(lastContext->childrenCount){
      uiElement **children = malloc(lastContext->childrenCount * sizeof(uiElement *));
      //LOG("offset %d\n", lastContext->offset);
      if(!children){
        LOG("!CRITICAL context allocation failed\n");
        exit(1);
      }
      for(int i = 0; i<lastContext->childrenCount; i++){
        uiElement *child = pop(&elements);
        children[i] = child;
      }
      LOG("%d\n", elements.top);
      uiElement *parent = peek(&elements);
      LOG("%d\n", lastContext->childrenCount);


      parent->numberOfChildren = lastContext->childrenCount;
      parent->children = children;
      LOG("HERE\n");
    LOG("attached to %p childrenCount %d\n", parent, parent->numberOfChildren);
    }
    LOG("context closed\n");
    free(lastContext);
  }
}

void attach(uiElement *element){
  contextInfo *lastContext = peek(&context);
  lastContext->childrenCount++;
  uiElement *parent = elements.s[lastContext->offset];
  element->relativeAccum.x = parent->relativeAccum.x + parent->bounds.x;
  element->relativeAccum.y = parent->relativeAccum.y + parent->bounds.y;
  element->parent = parent;

  LOG("parent acc %f %f\n", parent->relativeAccum.x, parent->relativeAccum.y);
  LOG("parent adress %p\n", parent);
  LOG("child topleft %f %f\n", element->bounds.x, element->bounds.y);

  push(element, &elements);

  LOG("element attached\n");
}

uiElement *rootAttach(){
  //uiElement *tmp = pop(&elements);
  //root = *tmp;
  free(pop(&context));
  return root;
}

// TODO: Collapse the BFS queue into a linear array, so that the BFS needs to be
// ran just once per UI scheme
// TODO: Clean this up obviously
void uiDrawUI(uiElement *uitree){
  queue uiDrawq;
  queueInit(&uiDrawq);
  uiElement *current;
  enqueue(uitree, &uiDrawq);
again:
  current = dequeue(&uiDrawq);
  for(int i = 0; i<current->numberOfChildren; i++){
    enqueue(current->children[i], &uiDrawq);
  }
  float textSizeAccum;
  GlyphInfo info;
  Font f = GetFontDefault();
  Vector2 offset = (Vector2){
    .x = current->parent?current->parent->bounds.x:0,
    .y = current->parent?current->parent->bounds.y:0
  };
  switch(current->type){
    case UI_NONE:
      //printf("current topleft %f %f\n", current.bounds.x, current.bounds.y);
      //printf("current acc %f %f\n", current.relativeAccum.x, current.relativeAccum.y);
      DrawRectangleLines(current->bounds.x + offset.x,
                         current->bounds.y + offset.y,
                         current->bounds.width, current->bounds.height, RED);
      break;
    case UI_TEXT:
      textSizeAccum = 0;
      for(int i = 0; current->bounds.x + textSizeAccum < current->bounds.x + current->bounds.width; i++){
        info = GetGlyphInfo(f, current->elementInItself.text.text[i]);
        DrawTextCodepoint(f, current->elementInItself.text.text[i], (Vector2){current->bounds.x + offset.x + textSizeAccum + f.glyphPadding, current->bounds.y + offset.y}, 20, GREEN);
        textSizeAccum += info.image.width*(20.0f/f.baseSize) + 1;
        if(!current->elementInItself.text.text[i+1]) break;
        //LOG("offset: %d\n", info.image.width);
        //LOG("glyph: %c\n", info.value);
        //LOG("glyphPad: %d\n", f.glyphPadding);
        //LOG("textSizeAccum %d\n", textSizeAccum);

      }
      //DrawText(current.elementInItself.text.text, current.bounds.x + current.relativeAccum.x, current.bounds.y + current.relativeAccum.y, 20, GREEN);
      break;
    case UI_BUTTON:
      break;
  }
  if(uiDrawq.front != uiDrawq.back) goto again;
}




