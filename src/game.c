#define LOG_PREFIX "Game"
#include "../include/common.h"
#include "../include/chess.h"
#include "../include/game.h"
#include "../include/stack.h"
#include "../include/chessUI.h"
#include "../include/connection.h"
#include <pthread.h>

static void(*moveP)(struct move *m);

extern struct board board;
extern struct drawInfo boardInfo;
extern int serverSocket;

static struct move *moves;
static stack moveStack;

pthread_mutex_t mutex;
int playingAsMem;

static const char *piece = " PBNRQK";

void printOutMoveList(){
  struct move *m;
  if(moveStack.s){
    LOG("Move list\n");
    for(int i = 0; i<moveStack.top; i++){
      m = (struct move *)moveStack.s[i];
      LOG("%c%c%d->%c%d\n", piece[m->who], m->from%8+'a', m->from/8+1, m->to%8+'a', m->to/8+1);
    }
  }
}

void moveLocal(struct move *m){
  struct move mlist[256];
  struct move *allocatedMove = bestow(sizeof(*allocatedMove));
  *allocatedMove = *m;

  board.halfMove++;
  if(board.board[m->from].type == PAWN) board.halfMove = 0;

  makeMove(allocatedMove);
  push(allocatedMove, &moveStack);

  // last move highlights
  for(int i = 0; i<64; i++){
    if(i == m->from || i == m->to){
      boardInfo.lastMoveArr[i] = 1;
    }
    else boardInfo.lastMoveArr[i] = 0;
  }

  if(board.board[m->to].side == 1) board.fullMove++;
  if(m->captured.type != NONE) board.halfMove = 0;

  if(!generateAllLegalMoves(mlist)){
    if(isPosAttacked(board.board, board.kingPos[board.currentSide], 1-board.currentSide)){
      board.state = CHECKMATE;
      LOG("Checkmate!\n");
      printOutMoveList();
    }
    else{
      board.state = DRAW;
      LOG("Stalemate!\n");
      printOutMoveList();
    }
  }
  if(board.halfMove == 100){
    board.state = DRAW;
    LOG("Draw by 50 move rule\n");
    printOutMoveList();
  }
}

void initGameLocal(){
  pthread_mutex_init(&mutex, NULL);
  boardInfo.playingAs = &board.currentSide;
  moveP = moveLocal;
  moves = bestow(50*sizeof(struct move *));
  stackInit(&moveStack, moves, 50);
}

void makeMoveSend(struct move *m){
  char buf[256];
  buf[0] = 'm';
  *(struct move *)(buf+1) = *m;
  sendC(serverSocket, &buf, sizeof(struct move) + 1, 0);
}

void makeMoveReceive(struct move *m){
  pthread_mutex_lock(&mutex);
  moveLocal(m);
  pthread_mutex_unlock(&mutex);
}

void initGameOnline(){
  pthread_mutex_init(&mutex, NULL);
  boardInfo.playingAs = &playingAsMem;
  moveP = makeMoveSend;
  moves = bestow(50*sizeof(struct move *));
  stackInit(&moveStack, moves, 50);
}

void move(struct move *m){
  moveP(m);
}


void cleanGameData(){
  if(moveStack.s){
    for(int i = 0; i<moveStack.top; i++){
      relinquish(moveStack.s[i]);
    }
    relinquish(moveStack.s);
  }

}
