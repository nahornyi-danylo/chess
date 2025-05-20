#include <raylib.h>
#include "../include/ui.h"
#define LOG_PREFIX "uitest"
#include "../include/common.h"
#include <stdio.h>

int windowWidth = 800;
int windowHeight = 600;

Vector2 mwheel = {0};

int main(){

// Load Cyrillic manually
// Clean up
//UnloadCodepoints(cyrillicCharset);
//Font font = LoadFontEx("/usr/share/fonts/TTF/DejaVuSans.ttf", 32, cyrillicCharset, charCount);


  uiElement *root = uiGetNone((Rectangle){.x = 0, .y = 0, .width = windowWidth, .height = windowHeight});
  InitWindow(windowWidth, windowHeight, "UI test");
  SetTargetFPS(60);

  Texture2D texture = LoadTexture("../resources/default/buttonTexture.png");

//int charCount = 0;
//int *cyrillicCharset = LoadCodepoints("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZабвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯІіїЇєЄґҐ", &charCount);
//
//// Now load font with that charset
//Font font = LoadFontEx("/usr/share/fonts/droid/DroidSans.ttf", 32, cyrillicCharset, charCount);
//printf("%d %d\n", font.texture.height, font.texture.width);    
//SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
//

  Shader sh;


  initUI(root, windowWidth, windowHeight);
  uiElement *text;

  uiOpenContext();
    uiAttach(text = uiGetNone((Rectangle){.x = 200, .y = 300, .width = 100, .height = 150}));
    uiOpenContext();
      uiAttach(uiGetNone((Rectangle){.x = 10, .y = 10, .width = 10, .height = 10}));
      uiAttach(uiGetNone((Rectangle){.x = 20, .y = 10, .width = 10, .height = 10}));
      uiAttach(uiGetText((Vector2){.x = -8, .y = 20}, "hello hello world", 20, RED));
      uiAttach(uiGetButton((Rectangle){40, 50, 100, 50}, sh, texture, NULL));
    uiCloseContext();
  uiCloseContext();

 uiScheme ui = uiFinalizeUI();
      // closeContext();
      // uiElement *tree;
      // tree = rootAttach();
      // printf("returned pointer %p\n", tree);

      while (!WindowShouldClose()) {
        BeginDrawing();
          ClearBackground(WHITE);
          uiHandleState(ui);
          uiDrawUI(ui);
          mwheel = GetMouseWheelMoveV();
          if(text->currentView.x != mwheel.x * 10){
            text->currentView.x += mwheel.x * 10;
          }
          if(text->currentView.y != mwheel.y * 10){
            text->currentView.y += mwheel.y * 10;
          }
        EndDrawing();
  }
  CloseWindow();
  uiDestroyUI(ui);
  LOG("alloc summary:\nallocations %d\nfrees %d\n", allocCounter, freeCounter);
}
