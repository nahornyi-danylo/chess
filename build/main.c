#include <raylib.h>
#include "../include/ui.h"
#define LOG_PREFIX "main"
#include "../include/common.h"
#include "../include/chess.h"
#include "../include/chessUI.h"
#include "../include/game.h"
#include "../include/connection.h"

#include <pthread.h>

extern struct board board;
extern struct drawInfo boardInfo;

int windowWidth = 500;
int windowHeight = 500;

pthread_t connectionThread;

int main(int argc, char **argv){
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  uiElement *root = uiGetNone((Rectangle){.x = 0, .y = 0, .width = windowWidth, .height = windowHeight});
  InitWindow(windowWidth, windowHeight, "chess");
  SetTargetFPS(60);


  initUI(root, windowWidth, windowHeight);
  if(argc == 3){
    initGameOnline();
    if(!initConnection(argv[1], argv[2])){
      pthread_create(&connectionThread, NULL, clientConnectionThread, NULL);
    }
    else{
      cleanGameData();
      initGameLocal();
    }
  }
  else{
    initGameLocal();
  }

  uiOpenContext();
    uiAttach(chessUIGetBoard((Rectangle){10, 10, 480, 480}, *boardInfo.playingAs));
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
