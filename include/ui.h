#include <raylib.h>

#ifndef UI_HEADER
#define UI_HEADER
typedef struct {
  float x;
  float y;
  float height;
  float width;
}RectangleNorm;

typedef enum {
  UI_NONE = 0,
  UI_BUTTON
}uiElementType;

typedef struct uiElement_{
  Rectangle bounds;
  // normalized in range 0 to 1, with center in top left corner
  RectangleNorm boundsNorm;
  Vector2 relativeAccum;

  // so it will be a switchcase probably
  // can't come up with anything better at the moment
  uiElementType type;
  void *elementInItself;

  int numberOfChildren;
  struct uiElement_ *children;
}uiElement;

typedef struct{
  int offset;
  int childrenCount;

}contextInfo;

typedef struct{
  bool isHovered;
  bool isDown;
  bool isPressed;

  Shader buttonShader;
  Texture buttonTexture;

  void (*callback)(void);
}uiButton;

// function protorypes
void initUI();
void attach(uiElement element);
void openContext();
void closeContext();
uiElement uiMakeButton();
void uiDrawUI();
void uiDestroyUI();
void rootAttach();
#endif
