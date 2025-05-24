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
  int count = 0;

  if (*str == '-') {
    return 0;
  }

  for (; *str && *str != ' '; str++) {
    switch (*str) {
      case 'K':
        board.castleMask |= CASTLE_WHITE_K;
        count++;
        break;
      case 'Q':
        board.castleMask |= CASTLE_WHITE_Q;
        count++;
        break;
      case 'k':
        board.castleMask |= CASTLE_BLACK_K;
        count++;
        break;
      case 'q':
        board.castleMask |= CASTLE_BLACK_Q;
        count++;
        break;
      default:
        return -1;
    }
  }

  return count;
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

  str++;
  while (isspace((unsigned char)*str)) str++;
  int n = parseCastleRights(str);
  if(n == -1) return -1;
  else str += n;

  //str++;
  if(*str == '-') board.enPassant = -1;
  else{
    board.enPassant = (str[1]-1) * 8 + str[0] - 'a';
    str+=2;
  }

  str++;
  if(*str && *str != ' '){
    sscanf(str, "%d %d", &board.halfMove, &board.fullMove);
  }
  else{
    board.halfMove = 0;
    board.fullMove = 1;
  }

  LOG("FEN parsed successfully\n");
  return 0;
fail:
  LOG("Error while loading FEN\n");
  return -1;
}

//enum pieceType{
  //NONE = 0,
  //PAWN,
  //BISHOP,
  //KNIGHT,
  //ROOK,
  //QUEEN,
  //KING
//};

static const char *correspond = "PBNRQKpbnrqk";

int getCurrentFEN(char *buf){
  int ret = 0;
  int p;
  int count = 0;
  for(int i = 7; i>=0; i--){
    for(int j = 0; j<8; j++){
      p = i*8+j;
      if(board.board[p].type == NONE){
        count++;
      }
      else{
        if(count) buf[ret++] = count + '0';
        buf[ret++] = correspond[board.board[p].type - 1 + 6 * board.board[p].side];
        count = 0;
      }
    }
    if(count) buf[ret++] = count + '0';
    count = 0;
    buf[ret++] = '/';
  }
  buf[ret++] = ' ';
  buf[ret++] = !board.currentSide?'w':'b';
  buf[ret++] = ' ';
  if(!board.castleMask){
    buf[ret++] = '-';
  }
  else{
    if(board.castleMask & CASTLE_WHITE_K) buf[ret++] = 'K';
    if(board.castleMask & CASTLE_WHITE_Q) buf[ret++] = 'Q';
    if(board.castleMask & CASTLE_BLACK_K) buf[ret++] = 'k';
    if(board.castleMask & CASTLE_BLACK_Q) buf[ret++] = 'q';
  }
  buf[ret++] = ' ';
  if(board.enPassant == -1) buf[ret++] = '-';
  else{
    buf[ret++] = board.enPassant%8 + 'a';
    buf[ret++] = board.enPassant/8 + 1;
  }
  buf[ret++] = ' ';

  ret += sprintf(&buf[ret], "%d %d", board.halfMove, board.fullMove);

  return ret;
}


