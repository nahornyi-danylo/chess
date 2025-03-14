#include  "../include/ui.h"
#include  "../include/stack.h"
#include "../include/queue.h"

#define LOG_PREFIX "[UI] "
#include "../include/common.h"


// TODO: generalize this all

static stack context;
static stack elements;

void initUI(uiElement *root, int width, int height){
  contextInfo *rootContext = malloc(sizeof(contextInfo));
  *rootContext = (contextInfo){0};

  contextInfo *contArray = malloc(10*sizeof(contextInfo));
  uiElement *elementArray = malloc(20*sizeof(uiElement));

  stackInit(&context, contArray, 10);
  stackInit(&elements, elementArray, 20);

  push(rootContext, &context);
  push(root, &elements);

  LOG("UI scheme initialized\n");
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

// TODO: handle incorrect call syntax
void openContext(){
  contextInfo *lastContext = NULL;
  contextInfo *newContext;

  if(context.top > 0){
    lastContext = peek(&context);
  }
  newContext = malloc(sizeof(contextInfo));

  if(!newContext){
    LOG("!CRITICAL context allocation failed\n");
    exit(1);
  }

  newContext->childrenCount = 0;
  newContext->offset = !lastContext?0:lastContext->offset+lastContext->childrenCount;

  push(newContext, &context);
}

void closeContext(){
  if(!context.top){
    LOG("No context to close\n");
    return;
  }
  contextInfo *lastContext = pop(&context);

  if(lastContext->childrenCount){
    uiElement **children = malloc(lastContext->childrenCount * sizeof(uiElement *));

    if(!children){
      LOG("!CRITICAL context allocation failed\n");
      exit(1);
    }

    for(int i = 0; i<lastContext->childrenCount; i++){
      uiElement *child = pop(&elements);
      children[i] = child;
    }
    uiElement *parent = peek(&elements);

    parent->numberOfChildren = lastContext->childrenCount;
    parent->children = children;
  }
  LOG("context closed\n");

  free(lastContext);
}

void attach(uiElement *element){
  if(!context.top){
    LOG("No context to attach to\n");
    return;
  }

  contextInfo *lastContext = peek(&context);
  uiElement *parent = elements.s[lastContext->offset];

  lastContext->childrenCount++;
  //element->relativeAccum.x = parent->relativeAccum.x + parent->bounds.x;
  //element->relativeAccum.y = parent->relativeAccum.y + parent->bounds.y;
  element->parent = parent;

  push(element, &elements);
  LOG("element attached\n");
}


// TODO: Collapse the BFS queue into a linear array, so that the BFS needs to be
// ran just once per UI scheme
// TODO: Clean this up obviously
void uiDrawUI(uiElement *uitree){
  queue uiDrawq;
  uiElement *current;
  Vector2 offset;
  queueInit(&uiDrawq);

  enqueue(uitree, &uiDrawq);

  while(uiDrawq.front != uiDrawq.back){
    current = dequeue(&uiDrawq);

    for(int i = 0; i<current->numberOfChildren; i++){
      enqueue(current->children[i], &uiDrawq);
    }
    //will be removed
    float textSizeAccum;
    GlyphInfo info;
    Font f = GetFontDefault();
    offset = (Vector2){
      .x = current->parent?current->parent->bounds.x:0,
      .y = current->parent?current->parent->bounds.y:0
    };

    switch(current->type){

      case UI_NONE:
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
        }
        break;

      case UI_BUTTON:
        break;
    }
  }
}




