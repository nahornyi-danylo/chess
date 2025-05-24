
#ifndef CHESS
#define CHESS
#define CASTLE_WHITE_K  0b0001
#define CASTLE_WHITE_Q  0b0010
#define CASTLE_BLACK_K  0b0100
#define CASTLE_BLACK_Q  0b1000

#define Q_SIDE_ROOK  0
#define K_SIDE_ROOK  7

enum pieceType{
  NONE = 0,
  PAWN,
  BISHOP,
  KNIGHT,
  ROOK,
  QUEEN,
  KING
};

struct piece{
  int side; // 0->white; 1->black 
  enum pieceType type;
};

enum gameState{
  ONGOING,
  DRAW,
  CHECKMATE
};

struct board{
  struct piece board[64];
  int kingPos[2];

  int currentSide;

  int enPassant;
  unsigned castleMask : 4;
  
  int halfMove;
  int fullMove;

  enum gameState state;
};

enum moveType{
  MOVE = 0,
  CAPTURE,
  EN_PASSANT,
  DOUBLE_PAWN,
  CASTLE_K,
  CASTLE_Q,
  PROMOTION_Q,
  PROMOTION_N,
  PROMOTION_R,
  PROMOTION_B
};


struct move{
  int from;
  int to;
  enum moveType type;
  enum pieceType who;
  struct piece captured;
  unsigned castlingRights : 4;
  int enPassant;
};
#endif


int isPosAttacked(struct piece *state, int pos, int opponent);
void makeMove(struct move *move);
void undoMove(struct move *move);
int tryMove(struct move *move);
int generateAllLegalMoves(struct move *mList);
int generateLegalMoves(struct move *mList, int pos);
int loadFEN(const char *str);
int getCurrentFEN(char *buf);

