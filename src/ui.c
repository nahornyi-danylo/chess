#include  "../include/ui.h"
#include  "../include/stack.h"
#include "../include/queue.h"

#define LOG_PREFIX "UI"
#include "../include/common.h"


// TODO: generalize this all

static stack context;
static stack elements;

void initUI(uiElement *root, int width, int height){
  contextInfo *rootContext = bestow(sizeof(contextInfo));
  *rootContext = (contextInfo){0};

  contextInfo *contArray = bestow(10*sizeof(contextInfo));
  uiElement *elementArray = bestow(20*sizeof(uiElement));

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
void uiOpenContext(){
  contextInfo *lastContext = NULL;
  contextInfo *newContext;

  if(context.top > 0){
    lastContext = peek(&context);
  }

  newContext = bestow(sizeof(contextInfo));
  newContext->childrenCount = 0;
  newContext->offset = !lastContext?0:lastContext->offset+lastContext->childrenCount;

  push(newContext, &context);
}

void uiCloseContext(){
  if(!context.top){
    LOG("No context to close\n");
    return;
  }
  contextInfo *lastContext = pop(&context);

  if(lastContext->childrenCount){
    uiElement **children = bestow(lastContext->childrenCount * sizeof(uiElement *));

    for(int i = 0; i<lastContext->childrenCount; i++){
      uiElement *child = pop(&elements);
      children[i] = child;
    }
    uiElement *parent = peek(&elements);

    parent->numberOfChildren = lastContext->childrenCount;
    parent->children = children;
  }
  LOG("context closed\n");

  relinquish(lastContext);
}

// TODO handle out-of-bounds behavior (scrollbars/cutoff)
void uiAttach(uiElement *element){
  if(!context.top){
    LOG("No context to attach to\n");
    return;
  }

  contextInfo *lastContext = peek(&context);
  uiElement *parent = elements.s[lastContext->offset];

  lastContext->childrenCount++;
  element->parent = parent;
  element->positionAbsolute.x += parent->positionAbsolute.x;
  element->positionAbsolute.y += parent->positionAbsolute.y;

  push(element, &elements);
  LOG("element attached\n");
}

uiScheme uiFinalizeUI(){
  uiScheme scheme = {0};

  scheme.tree = pop(&elements);
  uiElement *current = scheme.tree;
  relinquish(pop(&context));

  queue q;
  // I don't want to implement a standalone dynamic array right now, and it's basically the same
  // the pupropse is that I need to dequeue for BFS, and store it in another dyn array
  queue q2;
  queueInit(&q);
  queueInit(&q2);
  
  enqueue(current, &q);
  enqueue(current, &q2);
  
  int count = 1;
  while(q.front != q.back){
    current = dequeue(&q);

    for(int i = 0; i<current->numberOfChildren; i++){
      enqueue(current->children[i], &q);
      enqueue(current->children[i], &q2);
      count++;
    }
  }
  // the scheme now owns the pointer to that second queue
  scheme.elementCount = count;
  scheme.linear = (uiElement **)q2.q;
  relinquish(q.q);
  LOG("ui finalized\n");
  return scheme;
}

void uiDrawUI(uiScheme scheme){
  uiElement *current;

  for(int i = 0; i<scheme.elementCount; i++){
    current = scheme.linear[i];
    //should this be here?
    if(!current->draw){
      LOG("Draw function not provided\n");
      exit(1);
    }
    if(current->parent){
      BeginScissorMode(
        (int)current->parent->positionAbsolute.x,
        (int)current->parent->positionAbsolute.y,
        (int)current->parent->size.x,
        (int)current->parent->size.y
      );
    }
    current->draw(current);
    if(current->parent){
      EndScissorMode();
    }
  }
}

void uiHandleState(uiScheme scheme){
  Vector2 mousepos = GetMousePosition();
  unsigned permissions = 0b1111;

  uiElement *current = scheme.tree;
  // get the deepest element in the tree at mouse pos
  if(!current) return;
again:
  for(int i = 0; i<current->numberOfChildren; i++){
    if(current->children[i]){
      if (CheckCollisionPointRec(
              mousepos, (Rectangle){current->children[i]->positionAbsolute.x -
                                        current->currentView.x,
                                    current->children[i]->positionAbsolute.y -
                                        current->currentView.y,
                                    current->children[i]->size.x,
                                    current->children[i]->size.y})) {

        current = current->children[i];
        goto again;
      }
    }
  }
  
  // traverse back whilst calling handlers with appropriate permission mask
  while(current){
    if(permissions & current->handledState && current->handlerFunction){
      current->handlerFunction(permissions & current->handledState, current);
      permissions = permissions & ~current->handledState; 
    }

    current = current->parent;
  }


}

// should be all
void uiDestroyUI(uiScheme scheme){
  uiElement *current = scheme.tree;
  queue q;
  queueInit(&q);

  enqueue(current, &q);
  while(q.back != q.front){
    current = dequeue(&q);
    for(int i = 0; i<current->numberOfChildren; i++){
      enqueue(current->children[i], &q);
    }
    //the whole reason for BFSing all over again
    if(current->children) relinquish(current->children);
  }
  for(int i = 0; i<scheme.elementCount; i++){
    relinquish(scheme.linear[i]);
  }
  relinquish(q.q);
  relinquish(scheme.linear);
  relinquish(elements.s);
  relinquish(context.s);

  LOG("ui destroyed successfully\n");
}

// The following code will be of ui primitives in the following order:
// static draw function, possibly other related functions, and finally uiGet(element name) function

// UI_NONE, will be used for splitting the screen into regions
static void uiDrawNONE(uiElement *element){
  Vector2 view = {0};
  if(element->parent){
    view = element->parent->currentView;
  }
  // just a red outline for now
  DrawRectangleLines(element->positionAbsolute.x - view.x,
                     element->positionAbsolute.y - view.y,
                     element->size.x, element->size.y, RED);
}

uiElement *uiGetNone(Rectangle bounds){
  uiElement *result = bestow(sizeof(uiElement));
  *result = (uiElement){0};
  *result = (uiElement){
    .positionAbsolute.x = bounds.x,
    .positionAbsolute.y = bounds.y,
    .size.x = bounds.width,
    .size.y = bounds.height,
    .draw = uiDrawNONE
  };
  return result;
}

// UI_TEXT
static void uiDrawTEXT(uiElement *element){
  DrawTextEx(
      element->elementInItself.text.font, element->elementInItself.text.text,
      (Vector2){element->positionAbsolute.x - element->parent->currentView.x,
                element->positionAbsolute.y - element->parent->currentView.y},
      element->elementInItself.text.fontSize, 1,
      element->elementInItself.text.color);
}

// TODO: Load font with codepoints other then latin
uiElement *uiGetText(Vector2 position, char *text, float fontSize, Color color){
  Font font = LoadFont("/usr/share/fonts/droid/DroidSans.ttf");
  uiElement *result = bestow(sizeof(uiElement));
  *result = (uiElement){0};
  *result = (uiElement){
    .positionAbsolute = position,
    .elementInItself.text.text = text,
    .elementInItself.text.font = font,
    .elementInItself.text.color = color,
    .elementInItself.text.fontSize = fontSize,
    .elementInItself.text.scaleFactor = fontSize/font.baseSize,
    .draw = uiDrawTEXT
  };
  return result;
}


// UI_BUTTON
static void uiDrawBUTTON(uiElement *element){
  Vector2 view = {0};
  if(element->parent){
    view = element->parent->currentView;
  }
  DrawTexturePro(
      element->elementInItself.button.buttonTexture,
      (Rectangle){0, 0, element->elementInItself.button.buttonTexture.width,
                  element->elementInItself.button.buttonTexture.height},
      (Rectangle){element->positionAbsolute.x - view.x,
                element->positionAbsolute.y - view.y, element->size.x, element->size.y},
      (Vector2){0},
      0, WHITE);
};

static void uiButtonHandler(unsigned permissions, uiElement *element){
  if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
    LOG("button pressed\n");
  }
  if(element->elementInItself.button.callback){
    element->elementInItself.button.callback();
  }
}

uiElement *uiGetButton(Rectangle bounds, Shader shader, Texture2D texture, void(*func)(void)){
  uiElement *result = bestow(sizeof(uiElement));
  *result = (uiElement){0};
  *result = (uiElement){
    .positionAbsolute.x = bounds.x,
    .positionAbsolute.y = bounds.y,
    .size.x = bounds.width,
    .size.y = bounds.height,
    .draw = uiDrawBUTTON,
    .handledState = 0b1111,
    .handlerFunction = uiButtonHandler,
    .elementInItself.button.buttonShader = shader,
    .elementInItself.button.buttonTexture = texture,
    .elementInItself.button.callback = func
  };

  return result;
}



















