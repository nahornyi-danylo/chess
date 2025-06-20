#include <assert.h>
#include <time.h>
#include <ctype.h>
#include "../include/chess.h"
#define LOG_PREFIX "chess test"
#include "../include/common.h"

extern struct board board;
unsigned long long perft(int depth){
  struct move mlist[256];
  int n_moves;
  unsigned long long nodes = 0;

  if(depth == 0) return 1;

  n_moves = generateAllLegalMoves(mlist);

  for(int i = 0; i<n_moves; i++){
    makeMove(&mlist[i]);
    nodes += perft(depth-1);
    undoMove(&mlist[i]);
  }
  return nodes;
}

unsigned long long perft_divide(int depth) {
  struct move mlist[256];
  int n = generateAllLegalMoves(mlist);
  unsigned long long total = 0;

  LOG("———————\n");
  for (int i = 0; i < n; i++) {
    makeMove(&mlist[i]);
    unsigned long long count = perft(depth-1);

    undoMove(&mlist[i]);
    // Print from,to and count
    LOG("%c%d->%c%d  %8llu\n",
           mlist[i].from%8 + 'a', mlist[i].from/8+1, mlist[i].to%8 +'a', mlist[i].to/8+1, count);
    total += count;
  }
  LOG("———————\n");
  LOG(" Total: %8llu\n", total);

  return total;
}

void printBoard(){
  for(int i = 7; i>-1; i--){
    for(int j = 0; j<8; j++){
      char c;
      switch(board.board[i*8+j].type){
        case ROOK:
          c = 'r';
          break;
        case KNIGHT:
          c = 'n';
          break;
        case BISHOP:
          c = 'b';
          break;
        case QUEEN:
          c = 'q';
          break;
        case KING:
          c = 'k';
          break;
        case NONE:
          c = '0';
          break;
        case PAWN:
          c = 'p';
          break;

      }
      if(board.board[i*8+j].side == 0) c = toupper(c);
      printf("%c", c);
    }
    printf("\n");
  }

}

int main(){
  int time0 = time(NULL);
  char buf[128];
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  printBoard();
  assert(perft_divide(1) == 20);
  LOG("depth 1 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(2) == 400);
  LOG("depth 2 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(3) == 8902);
  LOG("depth 3 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(4) == 197281);
  LOG("depth 4 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(5) == 4865609);
  LOG("depth 5 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(6) == 119060324);
  LOG("depth 6 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  //assert(perft_divide(7) == 3195901860);
  //LOG("depth 7 passed and it took %lus\n", time(NULL)-time0);
  //time0 = time(NULL);
  //assert(perft_divide(8) == 84998978956);
  //LOG("depth 8 passed and it took %lus\n", time(NULL)-time0);
  //time0 = time(NULL);
  LOG("test for rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 passed successfully\n");

  loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  printBoard();
  assert(perft_divide(1) == 48);
  LOG("depth 1 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(2) == 2039);
  LOG("depth 2 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(3) == 97862);
  LOG("depth 3 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(4) == 4085603);
  LOG("depth 4 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(5) == 193690690);
  LOG("depth 5 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  //assert(perft_divide(6) == 8031647685);
  //LOG("depth 6 passed and it took %lus\n", time(NULL)-time0);
  //time0 = time(NULL);
  LOG("test for r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -  passed successfully\n");

  loadFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  printBoard();
  assert(perft_divide(1) == 14);
  LOG("depth 1 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(2) == 191);
  LOG("depth 2 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(3) == 2812);
  LOG("depth 3 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(4) == 43238);
  LOG("depth 4 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(5) == 674624);
  LOG("depth 5 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  assert(perft_divide(6) == 11030083);
  LOG("depth 6 passed and it took %lus\n", time(NULL)-time0);
  time0 = time(NULL);
  LOG("test for 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 passed successfully\n");
}
