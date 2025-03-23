#include <raylib.h>

#ifndef UI_HEADER
#define UI_HEADER

typedef enum{
  UI_NONE = 0,
  UI_TEXT,
  UI_BUTTON
}uiElementType;

typedef struct{
  bool isHovered;
  bool isDown;
  bool isPressed;

  Shader buttonShader;
  Texture buttonTexture;

  void (*callback)(void);
}uiButton;

typedef struct{
  Font font;
  Color color;
  float fontSize;

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
  
  // TBD 
  bool horizontallyScrollable;
  bool verticallyScrollable;

  int numberOfChildren;
  struct uiElement_ **children;

  struct uiElement_ *parent;

  // BEHOLD! A METHOD IN C!!
  void (*draw)(struct uiElement_ *);
}uiElement;

// tree representation for mouse hit testing and such
// linear representation so as not to call BFS on the full tree each draw call
typedef struct{
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
void uiDestroyUI(uiScheme scheme);
uiScheme uiFinailzeUI();

uiElement *uiGetNone(Rectangle bounds);
uiElement *uiGetText(Vector2 position, char *text, float fontSize, Color color);
