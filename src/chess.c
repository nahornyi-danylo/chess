#include "../include/chess.h"

struct board board = {0};

char *moveTypeRepr[] = {"MOVE", "CAPTURE", "PROMOTION_Q", "PROMOTION_N", "PROMOTION_R", "PROMOTION_B", "EN_PASSANT", "DOUBLE_PAWN", "CASTLE_K", "CASTLE_Q"};

static const int ROOK_DR[4] = { +1, -1,  0,  0 };
static const int ROOK_DF[4] = {  0,  0, +1, -1 };
static const int BISHOP_DR[4] = { +1, +1, -1, -1 };
static const int BISHOP_DF[4] = { +1, -1, +1, -1 };
static const int KNIGHT_DR[8] = { 2, 2, 1, 1, -1, -1, -2, -2 };
static const int KNIGHT_DF[8] = { 1, -1, 2, -2, 2, -2, 1, -1 };
static const int ADJACENT_DR[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
static const int ADJACENT_DF[8] = {-1, 0, 1, 1, -1, -1, 0, 1};

// 0 1 -> normal attacks 2 3 -> en passants
static const int PAWN_ATTACKS_DF[4] = {-1, 1, -1, 1};
static const int PAWN_ATTACKS_DR[4] = {1, 1, 0, 0};

#define isOnBoard(x) (x<64 && x>=0)
#define isOnBoard2d(x) (x<8 && x>=0)
#define isOnFile(pos, x) ()


int isPosAttacked(struct piece *state, int pos, int opponent){
  int posf = pos%8;
  int posr = pos/8;

  //check current rank and file for enemy rooks, or queens
  for(int i = 0; i < 4; i++){
    for(int f = posf + ROOK_DF[i], r = posr + ROOK_DR[i]; isOnBoard2d(f) && isOnBoard2d(r); f += ROOK_DF[i], r += ROOK_DR[i]){
      int j = r*8 + f;
      if(state[j].type != NONE){
        if (state[j].side == opponent &&
            (state[j].type == ROOK || state[j].type == QUEEN)){
          return 1;
        }
        break;
      }
    }
  }

  // check diagonals for bishops, or queens
  for(int i = 0; i < 4; i++){
    for (int f = posf + BISHOP_DF[i], r = posr + BISHOP_DR[i];
         isOnBoard2d(f) && isOnBoard2d(r);
         f += BISHOP_DF[i], r += BISHOP_DR[i]) {
      int j = r*8 + f;
      if(state[j].type != NONE){
        if (state[j].side == opponent &&
            (state[j].type == BISHOP || state[j].type == QUEEN)){

          return 1;
        }
        break;
      }
    }
  }

  // check for knights
  for(int i = 0; i<8; i++){
    int nr = pos/8 + KNIGHT_DR[i];
    int nf = pos%8 + KNIGHT_DF[i];
    if (nr < 0 || nr > 7 || nf < 0 || nf > 7) continue;
    int t = nr*8 + nf;
    if (isOnBoard(t) &&
      state[t].side == opponent &&
      state[t].type == KNIGHT){

      return 1;
    }
  }

  // check for opponent king
  for(int i = 0; i<8; i++){
    int f = posf + ADJACENT_DF[i];
    int r = posr + ADJACENT_DR[i];
    int t = r*8 + f;
    if (isOnBoard2d(f) && isOnBoard2d(r) && state[t].side == opponent && state[t].type == KING)
      return 1;
  }

  int oppPawnDir = opponent?1:-1;

  for(int i = 0; i<2; i++){
    int f = posf + PAWN_ATTACKS_DF[i] * oppPawnDir;
    int r = posr + PAWN_ATTACKS_DR[i] * oppPawnDir;
    int t = r*8 + f;
    if (isOnBoard2d(f) && isOnBoard2d(r) && state[t].side == opponent &&
        state[t].type == PAWN) {
      return 1;
    }
  }

  if(pos == board.enPassant){
    for(int i = 2; i<4; i++){
      int f = posf + PAWN_ATTACKS_DF[i] * oppPawnDir;
      int r = posr + PAWN_ATTACKS_DR[i] * oppPawnDir;
      int t = r*8 + f;

      if (isOnBoard2d(f) && isOnBoard2d(r) && state[t].side == opponent &&
          state[t].type == PAWN) {
        return 1;
      }
    }

  }

  return 0;
}

void makeMove(struct move *move){
  // save the snapshot of current castling rights and en passant candidate before the move
  move->castlingRights = board.castleMask;
  move->who = board.board[move->from].type;
  move->enPassant = board.enPassant;

  // here we don't bother checking for bounds as it is done in the candidate calculation stage
  int pawnDir = board.currentSide?-8:8;

  // handle castling rights wavering separately
  // if the king moves update the pos
  if(board.board[move->from].type == KING){
    board.kingPos[board.currentSide] = move->to;
    board.castleMask = board.castleMask & (0b0011 << (1-board.currentSide)*2);
  }
  else if(board.board[move->from].type == ROOK){
    if(move->from == Q_SIDE_ROOK + 56*board.currentSide){
      board.castleMask = board.castleMask & ~(0b0010 << (board.currentSide)*2);
    }
    else if(move->from == K_SIDE_ROOK + 56*board.currentSide){
      board.castleMask = board.castleMask & ~(0b0001 << (board.currentSide)*2);
    }
  }

  if(move->type != EN_PASSANT) board.enPassant = -1;
  switch(move->type){
    case DOUBLE_PAWN:
      board.enPassant = move->to; 
    case MOVE:
    case CAPTURE:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[move->from].type = NONE;
      break;
    case EN_PASSANT:
      move->captured = board.board[move->to];
      board.board[move->to + pawnDir] = board.board[move->from];
      board.board[move->to].type = NONE;
      board.board[move->from].type = NONE;
      break;
    case CASTLE_Q:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[Q_SIDE_ROOK + 56*board.currentSide].type = NONE;
      board.board[Q_SIDE_ROOK + 3 + 56*board.currentSide].type = ROOK;
      board.board[Q_SIDE_ROOK + 3 + 56*board.currentSide].side = board.currentSide;
      board.board[move->from].type = NONE;
      break;
    case CASTLE_K:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[K_SIDE_ROOK + 56*board.currentSide].type = NONE;
      board.board[K_SIDE_ROOK - 2 + 56*board.currentSide].type = ROOK;
      board.board[K_SIDE_ROOK - 2 + 56*board.currentSide].side = board.currentSide;
      board.board[move->from].type = NONE;
      break;
    case PROMOTION_Q:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[move->to].type = QUEEN;
      board.board[move->from].type = NONE;
      break;
    case PROMOTION_N:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[move->to].type = KNIGHT;
      board.board[move->from].type = NONE;
      break;
    case PROMOTION_R:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[move->to].type = ROOK;
      board.board[move->from].type = NONE;
      break;
    case PROMOTION_B:
      move->captured = board.board[move->to];
      board.board[move->to] = board.board[move->from];
      board.board[move->to].type = BISHOP;
      board.board[move->from].type = NONE;
      break;
  }



  board.currentSide = 1-board.currentSide;
}

void undoMove(struct move *move){
  board.currentSide = 1-board.currentSide;

  // restore caslting rights and en passant candidate
  board.castleMask = move->castlingRights;
  board.enPassant = move->enPassant;

  if(move->to == board.kingPos[board.currentSide]){
    board.kingPos[board.currentSide] = move->from;
  }

  int pawnDir = board.currentSide?-8:8;

  switch(move->type){
    case DOUBLE_PAWN:
    case MOVE:
    case CAPTURE:
      board.board[move->from] = board.board[move->to];
      board.board[move->to] = move->captured;
      break;
    case EN_PASSANT:
      board.board[move->to] = move->captured;
      board.board[move->to + pawnDir].type = NONE;
      board.board[move->from].type = PAWN;
      board.board[move->from].side = board.currentSide;
      break;
    case CASTLE_Q:
      board.board[move->from].type = KING;
      board.board[move->to].type = NONE;
      board.board[Q_SIDE_ROOK + 56*board.currentSide].type = ROOK;
      board.board[Q_SIDE_ROOK + 3 + 56*board.currentSide].type = NONE;
      break;
    case CASTLE_K:
      board.board[move->from].type = KING;
      board.board[move->to].type = NONE;
      board.board[K_SIDE_ROOK + 56*board.currentSide].type = ROOK;
      board.board[K_SIDE_ROOK - 2 + 56*board.currentSide].type = NONE;
      break;
    case PROMOTION_Q:
    case PROMOTION_N:
    case PROMOTION_R:
    case PROMOTION_B:
      board.board[move->to] = move->captured;
      board.board[move->from].type = PAWN;
      board.board[move->from].side = board.currentSide;
      break;
  }

}

int tryMove(struct move *move){

  int side = board.currentSide;
  makeMove(move);

  int attacked = isPosAttacked(board.board, board.kingPos[side], 1-side);

  undoMove(move);
  return attacked;
}

static int calculateCandidatesNone(int pos, struct move *mlist, int mlistIndex){
  return 0;
}

static int calculateCandidatesPawn(int pos, struct move *mlist, int mlistIndex){
  int color = board.board[pos].side;
  int opponent = 1-color;
  int startRank = (!color ? 1 : 6);
  int promoRank = (!color ? 6 : 1);
  int dir = !color?1:-1;

  struct move move = {0};
  move.from = pos;

  int posr = pos/8;
  int posf = pos%8;
  int idx = 0;

  for(int i = 0; i<2; i++){
    int f = posf + PAWN_ATTACKS_DF[i] * dir;
    int r = posr + PAWN_ATTACKS_DR[i] * dir;
    int t = r*8 + f;
    if (isOnBoard2d(f) && isOnBoard2d(r) && board.board[t].type != NONE) {
      if(board.board[t].side == opponent){
        if(posr == promoRank){
          move.type = CAPTURE;
          move.to = t;
          if(!tryMove(&move)){
            move.type = PROMOTION_Q;
            mlist[mlistIndex + idx++] = move;
            move.type = PROMOTION_N;
            mlist[mlistIndex + idx++] = move;
            move.type = PROMOTION_R;
            mlist[mlistIndex + idx++] = move;
            move.type = PROMOTION_B;
            mlist[mlistIndex + idx++] = move;
          }
          move = (struct move){0};
          move.from = pos;
        }
        else{
          move.type = CAPTURE;
          move.to = t;
          if(!tryMove(&move)){
            mlist[mlistIndex + idx++] = move;
          }
          move = (struct move){0};
          move.from = pos;
        }
      }
    }
  }

  for(int i = 2; i<4; i++){
    int f = posf + PAWN_ATTACKS_DF[i] * dir;
    int r = posr + PAWN_ATTACKS_DR[i] * dir;
    int t = r*8 + f;

    if (isOnBoard2d(f) && isOnBoard2d(r) && board.board[t].type != NONE) {
      if(board.board[t].side == opponent && t == board.enPassant){
        move.type = EN_PASSANT;
        move.to = t;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
    }
  }

  int t = (posr+dir)*8 + posf;
  int t1 = (posr+2*dir)*8 + posf;
  if(isOnBoard(t) && board.board[t].type == NONE){
    if(posr == promoRank){
      move.type = MOVE;
      move.to = t;
      if(!tryMove(&move)){
        move.type = PROMOTION_Q;
        mlist[mlistIndex + idx++] = move;
        move.type = PROMOTION_N;
        mlist[mlistIndex + idx++] = move;
        move.type = PROMOTION_R;
        mlist[mlistIndex + idx++] = move;
        move.type = PROMOTION_B;
        mlist[mlistIndex + idx++] = move;
      }
      move = (struct move){0};
      move.from = pos;
    }
    else{
      move.type = MOVE;
      move.to = t;
      if(!tryMove(&move)){
        mlist[mlistIndex + idx++] = move;
      }
      move = (struct move){0};
      move.from = pos;
    }
    if(posr == startRank){
      if(board.board[t1].type == NONE){
        move.type = DOUBLE_PAWN;
        move.to = t1;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
    }
  }

  return idx;
}

static int calculateCandidatesBishop(int pos, struct move *mlist, int mlistIndex){
  int opponent = 1-board.board[pos].side;

  struct move move = {0};
  move.from = pos;

  int posr = pos/8;
  int posf = pos%8;
  int idx = 0;

  for(int i = 0; i < 4; i++){
    for(int f = posf + BISHOP_DF[i], r = posr + BISHOP_DR[i]; isOnBoard2d(f) && isOnBoard2d(r); f += BISHOP_DF[i], r += BISHOP_DR[i]){
      int j = r*8 + f;
      if(board.board[j].type == NONE){
        move.type = MOVE;
        move.to = j;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
      else if(board.board[j].side == opponent){
        move.type = CAPTURE;
        move.to = j;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
        break;
      }
      else break;
    }
  }
  
  return idx;
}

static int calculateCandidatesKnight(int pos, struct move *mlist, int mlistIndex){
  int opponent = 1-board.board[pos].side;
  int idx = 0;

  struct move move = {0};
  move.from = pos;

  for(int d = 0; d<8; d++){
    int r = pos/8 + KNIGHT_DR[d];
    int f = pos%8 + KNIGHT_DF[d];
    int t = r*8 + f;
    if (isOnBoard2d(r) && isOnBoard2d(f)){
      if(board.board[t].type == NONE){
        move.type = MOVE;
        move.to = t;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
      else if(board.board[t].side == opponent){
        move.type = CAPTURE;
        move.to = t;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
    } 
  }


  return idx;
}

// yes, this is the same as for bishop, but with a different array.
// no, I'll not optimize this for the sake of having a func * array.
static int calculateCandidatesRook(int pos, struct move *mlist, int mlistIndex){
  int opponent = 1-board.board[pos].side;

  struct move move = {0};
  move.from = pos;

  int posr = pos/8;
  int posf = pos%8;
  int idx = 0;

  for(int i = 0; i < 4; i++){
    for(int f = posf + ROOK_DF[i], r = posr + ROOK_DR[i]; isOnBoard2d(f) && isOnBoard2d(r); f += ROOK_DF[i], r += ROOK_DR[i]){
      int j = r*8 + f;
      if(board.board[j].type == NONE){
        move.type = MOVE;
        move.to = j;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
      else if(board.board[j].side == opponent){
        move.type = CAPTURE;
        move.to = j;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
        break;
      }
      else break;
    }
  }
  
  return idx;
}

static int calculateCandidatesQueen(int pos, struct move *mlist, int mlistIndex){
  int i = calculateCandidatesBishop(pos, mlist, mlistIndex);
  i += calculateCandidatesRook(pos, mlist, mlistIndex + i);
  return i;
}

static int calculateCandidatesKing(int pos, struct move *mlist, int mlistIndex){
  int opponent = 1-board.board[pos].side;

  struct move move = {0};
  move.from = pos;

  int posr = pos/8;
  int posf = pos%8;
  int idx = 0;

  for(int i = 0; i < 8; i++){
    int f = posf + ADJACENT_DF[i];
    int r = posr + ADJACENT_DR[i];
    if(isOnBoard2d(f) && isOnBoard2d(r)){
      int t = r*8 + f;
      if(board.board[t].type == NONE){
        move.type = MOVE;
        move.to = t;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
      else if(board.board[t].side == opponent){
        move.type = CAPTURE;
        move.to = t;
        if(!tryMove(&move)){
          mlist[mlistIndex + idx++] = move;
        }
        move = (struct move){0};
        move.from = pos;
      }
    }
  }
  if(!isPosAttacked(board.board, pos, opponent)){
    // king side
    if(board.castleMask & (CASTLE_WHITE_K<<(2*(1-opponent)))){
      if (board.board[posr*8 + posf+1].type == NONE &&
          board.board[posr*8 + posf+2].type == NONE &&
          board.board[posr*8 + posf+3].type == ROOK &&
          !isPosAttacked(board.board, posr * 8 + posf + 1, opponent) &&
          !isPosAttacked(board.board, posr * 8 + posf + 2, opponent)){
        move.type = CASTLE_K;
        move.to = posr * 8 + posf + 2;
        mlist[mlistIndex + idx++] = move;
        move = (struct move){0};
        move.from = pos;
      }
    }
    if(board.castleMask & (CASTLE_WHITE_Q<<(2*(1-opponent)))){
      if (board.board[posr*8 + posf-1].type == NONE &&
          board.board[posr*8 + posf-2].type == NONE &&
          board.board[posr*8 + posf-3].type == NONE &&
          board.board[posr*8 + posf-4].type == ROOK &&
          !isPosAttacked(board.board, posr * 8 + posf - 1, opponent) &&
          !isPosAttacked(board.board, posr * 8 + posf - 2, opponent)){
        move.type = CASTLE_Q;
        move.to = posr * 8 + posf - 2;
        mlist[mlistIndex + idx++] = move;
        move = (struct move){0};
        move.from = pos;
      }
    }
  }

  return idx;
}

int (*calculateCandidates[])(int, struct move *, int) = {
  calculateCandidatesNone,
  calculateCandidatesPawn,
  calculateCandidatesBishop,
  calculateCandidatesKnight,
  calculateCandidatesRook,
  calculateCandidatesQueen,
  calculateCandidatesKing
};


int generateAllLegalMoves(struct move *mList){
  int accum = 0;
  for(int i = 0; i<64; i++){
    if(board.board[i].type != NONE && board.board[i].side == board.currentSide){
      accum += calculateCandidates[board.board[i].type](i, mList, accum);
    }
  }

  return accum;
}

int generateLegalMoves(struct move *mList, int pos){
  int accum = 0;
  if(board.board[pos].type != NONE && board.board[pos].side == board.currentSide){
    accum = calculateCandidates[board.board[pos].type](pos, mList, accum);
  }

  return accum;
}
