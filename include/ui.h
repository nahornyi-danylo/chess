#include <raylib.h>

#ifndef UI_HEADER
#define UI_HEADER
typedef struct{
  float x;
  float y;
  float height;
  float width;
}RectangleNorm;

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
  GlyphInfo info;
  float sizeScalingFactor;

  char *text;
}uiText;

typedef struct uiElement_{
  Rectangle bounds;
  Vector2 currentView;

  // normalized in range 0 to 1, with center in top left corner
  RectangleNorm boundsNorm;
  Vector2 relativeAccum;

  uiElementType type;
  union{
    uiText text;
    uiButton button;
  }elementInItself;

  bool horizontallyScrollable;
  bool verticallyScrollable;

  int numberOfChildren;
  struct uiElement_ **children;

  struct uiElement_ *parent;
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


// function protorypes
void initUI(uiElement *root, int width, int height);
void uiAttach(uiElement *element);
void uiOpenContext();
void uiCloseContext();
uiElement uiMakeButton();
void uiDrawUI(uiScheme scheme);
void uiDestroyUI(uiScheme scheme);
uiScheme uiFinailzeUI();
#endif
