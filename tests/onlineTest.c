#include <raylib.h>
#include "../include/ui.h"
#define LOG_PREFIX "board test"
#include "../include/common.h"
#include "../include/chess.h"
#include "../include/chessUI.h"
#include "../include/game.h"
#include "../include/connection.h"

#include <pthread.h>

extern struct board board;
extern int *playingAs;

int windowWidth = 800;
int windowHeight = 600;

pthread_t connectionThread;

int main(int argc, char **argv){
  if(argc != 3) exit(1);
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  uiElement *root = uiGetNone((Rectangle){.x = 0, .y = 0, .width = windowWidth, .height = windowHeight});
  InitWindow(windowWidth, windowHeight, "board test");
  SetTargetFPS(60);


  initUI(root, windowWidth, windowHeight);
  initGameOnline();
  initConnection(argv[1], argv[2]);
  pthread_create(&connectionThread, NULL, clientConnectionThread, NULL);

  uiOpenContext();
    uiAttach(chessUIGetBoard((Rectangle){10, 10, 500, 500}, *playingAs));
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
