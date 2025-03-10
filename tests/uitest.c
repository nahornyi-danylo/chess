#include <raylib.h>
#include "../include/ui.h"

int windowWidth = 800;
int windowHeight = 600;

int main(){
  uiElement test = (uiElement){
    .type = UI_NONE,
    .children = 0,
    .numberOfChildren = 0,
    .elementInItself = 0,
    .bounds = (Rectangle){.x = 100, .y = 100, .width = 100, .height = 100},
    .relativeAccum= (Vector2){.x = 0, .y = 0}
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
  InitWindow(windowWidth, windowHeight, "UI test");
  SetTargetFPS(60);
  initUI();
  openContext();
  attach(test);
  openContext();
  attach(test2);
  attach(test3);
  closeContext();
  closeContext();
  rootAttach();

  while (!WindowShouldClose()){
    BeginDrawing();
    uiDrawUI();
    EndDrawing();
  }
  CloseWindow();
}
