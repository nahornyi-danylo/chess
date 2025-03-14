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

typedef struct{
  int offset;
  int childrenCount;

}contextInfo;


// function protorypes
void initUI(uiElement *root, int width, int height);
void attach(uiElement *element);
void openContext();
void closeContext();
uiElement uiMakeButton();
void uiDrawUI(uiElement *uitree);
void uiDestroyUI();
uiElement *rootAttach();
#endif
