#define LOG_PREFIX "Game"
#include "../include/common.h"
#include "../include/chess.h"
#include "../include/game.h"
#include "../include/stack.h"
#include "../include/connection.h"
#include <pthread.h>

static void(*moveP)(struct move *m);

extern struct board board;
static struct move *moves;
static stack moveStack;

extern int serverSocket;
extern pthread_mutex_t mutex;
int *playingAs;
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

void move(struct move *m){
  struct move mlist[256];
  struct move *allocatedMove = bestow(sizeof(*allocatedMove));
  *allocatedMove = *m;

  moveP(allocatedMove);
  push(allocatedMove, &moveStack);
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
}

void initGameLocal(){
  playingAs = &board.currentSide;
  moveP = makeMove;
  moves = bestow(50*sizeof(struct move *));
  stackInit(&moveStack, moves, 50);
}

void makeMoveSend(struct move *m){
  struct msgM msg = {'m', *m};
  sendC(serverSocket, &msg, sizeof(struct msgM), 0);
}

void makeMoveReceive(struct move *m){
  pthread_mutex_lock(&mutex);
  moveP = makeMove;
  move(m);
  moveP = makeMoveSend;
  pthread_mutex_unlock(&mutex);

}

void initGameOnline(){
  playingAs = &playingAsMem;
  moveP = makeMoveSend;
  moves = bestow(50*sizeof(struct move *));
  stackInit(&moveStack, moves, 50);
}


void cleanGameData(){
  if(moveStack.s){
    for(int i = 0; i<moveStack.top; i++){
      relinquish(moveStack.s[i]);
    }
    relinquish(moveStack.s);
  }

}
