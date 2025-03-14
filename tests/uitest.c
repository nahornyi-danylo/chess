#include <raylib.h>
#include "../include/ui.h"
#include <stdio.h>

int windowWidth = 800;
int windowHeight = 600;

int main(){
  uiElement root = {0};
  root = (uiElement){
    .bounds.width = windowWidth,
    .bounds.height = windowHeight
  };
  uiElement test = (uiElement){
    .type = UI_NONE,
    .children = 0,
    .numberOfChildren = 0,
    .elementInItself = 0,
    .bounds = (Rectangle){.x = 200, .y = 300, .width = 100, .height = 150},
    .relativeAccum= (Vector2){.x = 0, .y = 0},
    .horizontallyScrollable = true
  };
  uiElement test2 = (uiElement){
    .type = UI_NONE,
    .children = 0,
    .numberOfChildren = 0,
    .elementInItself = 0,
    .bounds = (Rectangle){.x = 10, .y = 10, .width = 10, .height = 10},
    .relativeAccum= (Vector2){.x = 0, .y = 0}
  };
  uiElement test3 = (uiElement){
    .type = UI_NONE,
    .children = 0,
    .numberOfChildren = 0,
    .elementInItself = 0,
    .bounds = (Rectangle){.x = 20, .y = 10, .width = 10, .height = 10},
    .relativeAccum= (Vector2){.x = 0, .y = 0}
  };
  uiElement test4 = (uiElement){
    .type = UI_TEXT,
    .children = 0,
    .numberOfChildren = 0,
    .elementInItself = (uiText){.text = "hello world"},
    .bounds = (Rectangle){.x = 30, .y = 10, .width = test.bounds.width-30, .height = test.bounds.height-10},
    .relativeAccum= (Vector2){.x = 0, .y = 0}
  };
  InitWindow(windowWidth, windowHeight, "UI test");
  SetTargetFPS(60);
  initUI(&root, windowWidth, windowHeight);
  uiOpenContext();
    uiAttach(&test);
    uiOpenContext();
      uiAttach(&test2);
      uiAttach(&test3);
      uiAttach(&test4);
    uiCloseContext();
  uiCloseContext();

  uiScheme ui = uiFinailzeUI();
  //closeContext();
  //uiElement *tree;
  //tree = rootAttach();
  //printf("returned pointer %p\n", tree);

  while (!WindowShouldClose()){
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
      test.bounds.x = GetMouseX();
      test.bounds.y = GetMouseY();
    }
    BeginDrawing();
    ClearBackground(BLACK);
    uiDrawUI(ui);
    EndDrawing();
  }
  CloseWindow();
  uiDestroyUI(ui);
}
