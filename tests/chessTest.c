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

void drawBoard(){
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
  char buf[128];
  int n = 0;
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  n = getCurrentFEN(buf);
  for(int i = 0; i<n; i++){
    printf("%c", buf[i]);
  }
  printf("\n");

  drawBoard();
  //perft_divide(7);

  loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  n = getCurrentFEN(buf);
  for(int i = 0; i<n; i++){
    printf("%c", buf[i]);
  }
  printf("\n");
  drawBoard();
  //perft_divide(6);
}
