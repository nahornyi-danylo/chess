#include <raylib.h>
#include "../include/ui.h"
#define LOG_PREFIX "board test"
#include "../include/common.h"
#include "../include/chess.h"
#include "../include/chessUI.h"
#include "../include/game.h"
#include <stdio.h>

extern struct board board;

int windowWidth = 800;
int windowHeight = 600;

int main(){
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  //loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  uiElement *root = uiGetNone((Rectangle){.x = 0, .y = 0, .width = windowWidth, .height = windowHeight});
  InitWindow(windowWidth, windowHeight, "board test");
  SetTargetFPS(60);

  initUI(root, windowWidth, windowHeight);
  initGameLocal();
  

  uiOpenContext();
    uiAttach(chessUIGetBoard((Rectangle){10, 10, 500, 500}, 0));
  uiCloseContext();

  uiScheme ui = uiFinalizeUI();
  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground(WHITE);
      uiHandleState(ui);
      uiDrawUI(ui);
    EndDrawing();
  }
  CloseWindow();
  uiDestroyUI(ui);
  cleanGameData();
  LOG("alloc summary:\nallocations %d\nfrees %d\n", allocCounter, freeCounter);
}

