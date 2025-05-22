#include <raylib.h>

#ifndef UI_HEADER
#define UI_HEADER

typedef enum{
  UI_NONE = 0,
  UI_TEXT,
  UI_BUTTON
}uiElementType;

typedef struct{
  Shader buttonShader;
  Texture buttonTexture;

  void (*callback)(void);
}uiButton;


typedef struct{
  Font font;
  Color color;
  float fontSize;
  float scaleFactor;

  char *text;
}uiText;

typedef struct uiElement_{
  // Might look messy in code but still... and relative position can be infered
  // from parent if need be
  Vector2 positionAbsolute;
  Vector2 currentView;
  Vector2 size;

  // normalized in range 0 to 1, with center in top left corner unused for now
  Rectangle boundsNorm;

  // still keeping this for potential use in resizes and such
  uiElementType type;
  union{
    uiText text;
    uiButton button;
  }elementInItself;
  
  // bitmap of state handlers
  // from msb to lsb: mouse presses, m wheel up m wheel down, hover.
  // when such an event occurs, to the elements on the current mouse pos
  // is given the contrlot flow for maximum flexibility, from leaf nodes to roots
  // in that order. If an event is handled at a higher depth, the corresponding
  // bits are subtracted, such that if both parent and child nodes handle, say,
  // mouse wheel change, only the child node is permitted to do so. However,
  // these restrictions are not inforced. The handler function gets the
  // uiElement *, and the allowed bitmap.
  unsigned handledState : 4;
  void (*handlerFunction)(unsigned, struct uiElement_ *);


  int numberOfChildren;
  struct uiElement_ **children;

  struct uiElement_ *parent;

  void (*draw)(struct uiElement_ *);
}uiElement;

typedef struct{
  uiElement *toHovered;
  uiElement *toPress;
  uiElement *toScroll;

}state;

// tree representation for mouse hit testing and such
// linear representation so as not to call BFS on the full tree each draw call
typedef struct{
  state currentState;
  uiElement *tree;

  int elementCount;
  uiElement **linear;
}uiScheme;

typedef struct{
  int offset;
  int childrenCount;

}contextInfo;

#endif

// function protorypes
void initUI(uiElement *root, int width, int height);
void uiAttach(uiElement *element);
void uiOpenContext();
void uiCloseContext();
uiElement uiMakeButton();
void uiDrawUI(uiScheme scheme);
void uiHandleState(uiScheme scheme);
void uiDestroyUI(uiScheme scheme);
uiScheme uiFinalizeUI();

uiElement *uiGetNone(Rectangle bounds);
uiElement *uiGetText(Vector2 position, char *text, float fontSize, Color color);
uiElement *uiGetButton(Rectangle bounds, Shader shader, Texture2D texture, void(*func)(void));
