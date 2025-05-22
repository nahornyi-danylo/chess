#include <ctype.h>
#include "../include/chess.h"

#define LOG_PREFIX "FEN parser"
#include "../include/common.h"

extern struct board board;

static const char *FENpcs = "rnbqkpRNBQKP";
static const struct piece FENmap[12] = {
    {1, ROOK},   {1, KNIGHT}, {1, BISHOP},
    {1, QUEEN},  {1, KING},   {1, PAWN},
    {0, ROOK},   {0, KNIGHT}, {0, BISHOP},
    {0, QUEEN},  {0, KING},   {0, PAWN},
};

// returns 0..11 for a piece, or –1 if not a piece char
static int fenPieceId(char c) {
  for (int i = 0; i < 12; i++){
    if (FENpcs[i] == c) return i;
  }
  return -1;
}

int parseFENpieces(const char *fen) {
  for (int rank = 7; rank >= 0; rank--) {
    int file = 0;
    while (*fen && *fen != '/' && !isspace((unsigned char)*fen)) {
      if (isdigit((unsigned char)*fen)) {
        int empty = *fen - '0';
        if (empty < 1 || empty > 8) return -1;
        for (int i = 0; i < empty; i++, file++) {
          int sq = rank*8 + file;
          board.board[sq].type = NONE;
        }
      }
      else {
        int id = fenPieceId(*fen);
        if (id < 0) return -1;
        int sq = rank*8 + file;
        board.board[sq] = FENmap[id];
        // if this is a king, record its square
        if (FENmap[id].type == KING) {
          board.kingPos[ FENmap[id].side ] = sq;
        }
        file++;
      }
      fen++;
      if (file > 8) return -1;  // too many files
    }
    if (file != 8) return -1;   // each rank must have exactly 8 squares
    if (*fen == '/') fen++;
    else if (isspace((unsigned char)*fen)) break;
    else return -1;
  }
  return 0;
}

int parseCastleRights(const char *str) {
  board.castleMask = 0b0000;

  if (*str == '-') {
    return 0;
  }

  for (; *str && *str != ' '; str++) {
    switch (*str) {
      case 'K':
        board.castleMask |= CASTLE_WHITE_K;
        break;
      case 'Q':
        board.castleMask |= CASTLE_WHITE_Q;
        break;
      case 'k':
        board.castleMask |= CASTLE_BLACK_K;
        break;
      case 'q':
        board.castleMask |= CASTLE_BLACK_Q;
        break;
      default:
        return -1;
    }
  }
  LOG("%d\n", board.castleMask);

  return 0;
}

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
int loadFEN(const char *str){
  if(parseFENpieces(str)){
    goto fail;
  };
  while(*str!=' ') str++;
  str++;

  if(*str == 'w'){
    board.currentSide = 0;
  }
  else if(*str == 'b'){
    board.currentSide = 1;
  }
  else goto fail;
  LOG("%d\n", board.currentSide);

  str++;
  while (isspace((unsigned char)*str)) str++;
  parseCastleRights(str);

  // TODO finish this
  

  return 0;
fail:
  LOG("Error while loading FEN");
  return -1;
}
