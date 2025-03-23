#include <raylib.h>
#include "../include/ui.h"
#include <stdio.h>

int windowWidth = 800;
int windowHeight = 600;

Vector2 mwheel = {0};

int main(){
  uiElement *root = uiGetNone((Rectangle){.x = 0, .y = 0, .width = windowWidth, .height = windowHeight});
  InitWindow(windowWidth, windowHeight, "UI test");
  SetTargetFPS(60);
  initUI(root, windowWidth, windowHeight);
  uiElement *text;

  uiOpenContext();
    uiAttach(text = uiGetNone((Rectangle){.x = 200, .y = 300, .width = 100, .height = 150}));
    uiOpenContext();
      uiAttach(uiGetNone((Rectangle){.x = 10, .y = 10, .width = 10, .height = 10}));
      uiAttach(uiGetNone((Rectangle){.x = 20, .y = 10, .width = 10, .height = 10}));
      uiAttach(uiGetText((Vector2){.x = -8, .y = 20}, "hello world hello world", 20, RED));
    uiCloseContext();
  uiCloseContext();

      uiScheme ui = uiFinailzeUI();
      // closeContext();
      // uiElement *tree;
      // tree = rootAttach();
      // printf("returned pointer %p\n", tree);

      while (!WindowShouldClose()) {
        BeginDrawing();
          ClearBackground(BLACK);
          uiDrawUI(ui);
          mwheel = GetMouseWheelMoveV();
          if(text->currentView.x != mwheel.x * 10){
            printf("%f\n", mwheel.x);
            text->currentView.x += mwheel.x * 10;
          }
        EndDrawing();
  }
  CloseWindow();
  uiDestroyUI(ui);
}
