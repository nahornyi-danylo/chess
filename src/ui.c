#include  "../include/ui.h"
#include  "../include/stack.h"
#include "../include/queue.h"

#define LOG_PREFIX "[UI] "
#include "../include/common.h"


// TODO: generalize this all

static stack context;
static stack elements;

// Oh the allmighty Gods of the GNU C compiler I pray to thee
// allow this meager and insignificant function to be inlined
static inline Vector2 add2Vec2(Vector2 one, Vector2 two){
  return (Vector2){
    one.x + two.x,
    one.y + two.y
  };
}

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
void uiOpenContext(){
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

void uiCloseContext(){
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

uiScheme uiFinailzeUI(){
  uiScheme scheme = {0};

  scheme.tree = pop(&elements);
  uiElement *current = scheme.tree;
  free(pop(&context));

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
  free(q.q);
  LOG("ui finalized\n");
  return scheme;
}

// TODO: Clean this up obviously
void uiDrawUI(uiScheme scheme){
  Vector2 offset;
  uiElement *current;

  for(int i = 0; i<scheme.elementCount; i++){
    current = scheme.linear[i];
    //should this be here?
    if(!current->draw){
      LOG("Draw function not provided\n");
      exit(1);
    }
    current->draw(current);
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
    free(current->children);
  }
  for(int i = 0; i<scheme.elementCount; i++){
    free(scheme.linear[i]);
  }
  free(q.q);
  free(scheme.linear);
  free(elements.s);
  free(context.s);

  LOG("ui destroyed successfully\n");
}

// The following code will be of ui primitives in the following order:
// static draw function, possibly other related functions, and finally uiGet(element name) function

// UI_NONE, will be used for splitting the screen into regions
static void uiDrawNONE(uiElement *element){
  // just a red outline for now
  DrawRectangleLines(element->positionAbsolute.x,
                     element->positionAbsolute.y,
                     element->size.x, element->size.y, RED);
}

uiElement *uiGetNone(Rectangle bounds){
  uiElement *result = malloc(sizeof(uiElement));
  if(!result){
    LOG("element NONE allocation failed");
    exit(1);
  
  }
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

// Same as the raylib implementation, but modified to be bound to a rect with
// consideration for the current view 
// returns 0 if the out of bounds, as a signal to stop feeding chars that won't be drawn
// TODO handle for vertical bounds
int DrawTextCodepointBound(Font font, int codepoint, float *offsetAccum, Rectangle bounds, Vector2 view, Vector2 position, float fontSize, Color tint){
  // Character index position in sprite font
  // NOTE: In case a codepoint is not available in the font, index returned points to '?'
  int index = GetGlyphIndex(font, codepoint);
  float scaleFactor = fontSize/font.baseSize;     // Character quad scaling factor
  float leftDiff = 0;
  float rightDiff = 0;
  Rectangle glyphUnscaled = {
    font.recs[index].x - (float)font.glyphPadding,
    font.recs[index].y - (float)font.glyphPadding,
    font.recs[index].width + 2.0f * font.glyphPadding,
    font.recs[index].height + 2.0f * font.glyphPadding
  };

  Rectangle glyphScaled = {
    font.glyphs[index].offsetX * scaleFactor - (float)font.glyphPadding * scaleFactor,
    font.glyphs[index].offsetY * scaleFactor - (float)font.glyphPadding * scaleFactor,
    glyphUnscaled.width * scaleFactor,
    glyphUnscaled.height * scaleFactor
  };

  if(glyphScaled.width + position.x + *offsetAccum < bounds.x ||
      position.x + *offsetAccum > bounds.x + bounds.width) {
    *offsetAccum += font.recs[index].width * scaleFactor + font.glyphPadding * 2 + 1;
    return 0;
  }
  
  leftDiff = bounds.x - (position.x + *offsetAccum) > 0?bounds.x - (position.x + *offsetAccum):0;
  rightDiff = position.x + *offsetAccum + glyphScaled.width - (bounds.x + bounds.width) > 0?position.x + *offsetAccum + glyphScaled.width - (bounds.x + bounds.width):0;

  // Character destination rectangle on screen
  // NOTE: We consider glyphPadding on drawing
  Rectangle dstRec = {
    position.x + *offsetAccum + glyphScaled.x + leftDiff - view.x,
    position.y + glyphScaled.y,
    glyphScaled.width - leftDiff - rightDiff,
    glyphScaled.height
  };

  // Character source rectangle from font texture atlas
  // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
  Rectangle srcRec = {
    glyphUnscaled.x + leftDiff/scaleFactor,
    glyphUnscaled.y,
    glyphUnscaled.width - leftDiff/scaleFactor - rightDiff/scaleFactor,
    glyphUnscaled.height
  };

  // Draw the character texture on the screen
  DrawTexturePro(font.texture, srcRec, dstRec, (Vector2){ 0, 0 }, 0.0f, tint);
  *offsetAccum += font.recs[index].width * scaleFactor + font.glyphPadding * 2 + 1;
  return 1;
}

static void uiDrawTEXT(uiElement *element){
  GlyphInfo info;
  float offsetAccum = 0;
  Vector2 currentBoundView = add2Vec2(element->parent->positionAbsolute, element->parent->currentView);
  for(int i = 0; element->elementInItself.text.text[i]; i++){
    info = GetGlyphInfo(element->elementInItself.text.font, element->elementInItself.text.text[i]);
    DrawTextCodepointBound(
        element->elementInItself.text.font,
        element->elementInItself.text.text[i], &offsetAccum,
        (Rectangle){currentBoundView.x, currentBoundView.y,
                    element->parent->size.x, element->parent->size.y},
        element->parent->currentView,
        element->positionAbsolute, element->elementInItself.text.fontSize,
        element->elementInItself.text.color);
  }
}

uiElement *uiGetText(Vector2 position, char *text, float fontSize, Color color){
  uiElement *result = malloc(sizeof(uiElement));
  if(!result){
    LOG("element NONE allocation failed");
    exit(1);
  }

  *result = (uiElement){0};
  *result = (uiElement){
    .positionAbsolute = position,
    .elementInItself.text.text = text,
    .elementInItself.text.font = GetFontDefault(),
    .elementInItself.text.color = color,
    .elementInItself.text.fontSize = fontSize,
    .draw = uiDrawTEXT
  };
  return result;
}





















