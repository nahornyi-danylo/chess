#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOG_PREFIX "server"
#include "../include/common.h"
#include "../include/connection.h"
#include "../include/chess.h"
#include "../include/game.h"

#define PORT "12345"

extern struct board board;

int mainSocket = 0;
int clientSockets[2] = {};
struct sockaddr_storage clientInfo[2];

static void initConnectionServ(){
  socklen_t len = sizeof(clientInfo[0]);
  int error;
  int yes = 1;
  char s[INET_ADDRSTRLEN];

  struct addrinfo hints = {0};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *servinfo;
  struct addrinfo *p;

  if((error = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
    LOG("getaddrinfo: %s\n", gai_strerror(error));
    exit(1);
  }

  for(p = servinfo; p != NULL; p = p->ai_next){
    if((mainSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      LOG("main socket error:\n");
      perror("");
      continue;
    }

    if(setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      LOG("setsockopt error:\n");
      perror("");
      exit(1);
    }

    if(bind(mainSocket, p->ai_addr, p->ai_addrlen) == -1){
      LOG("binding error:\n");
      perror("");
      exit(1);
    }

    break;
  }

  freeaddrinfo(servinfo);

  if(listen(mainSocket, 2) == -1){
    LOG("fail on listen:\n");
    perror("");
    exit(1);
  }

  // connection 1
    clientSockets[0] = accept(mainSocket, (struct sockaddr *)&clientInfo[0], &len);
    inet_ntop(clientInfo[0].ss_family, &((struct sockaddr_in *)&clientInfo[0])->sin_addr, s, sizeof(s));
    LOG("connection %s\n", s);
  // connection 2
    clientSockets[1] = accept(mainSocket, (struct sockaddr *)&clientInfo[1], &len);
    inet_ntop(clientInfo[1].ss_family, &((struct sockaddr_in *)&clientInfo[1])->sin_addr, s, sizeof(s));
    LOG("connection %s\n", s);
}

// server->client:
// i - init message, tells which side to assume
// m - move, made by opponent and approved by server
// d - move not approved, includes the current FEN position for client to correct itself
// a - approved
// f game finished, includes a char 0 1 or 2 for white, black, and draw


struct move mlist[256];
int n = 0;

// client can only ever send a move
int interpretMsgServ(char *str){
  int approve = 0;
  // not sure which side the char will get padded
  struct msgM *msg = (struct msgM *)str;
  if(msg->msgType != 'm') return 0;
  struct move *m = &msg->move;
  for(int i = 0; i<n; i++){
    // I believe these 3 fields are enough
    if(m->to == mlist[i].to &&
       m->from == mlist[i].from &&
       m->type == mlist[i].type){
      LOG("Valid move %d->%d\n", m->from, m->to);
      approve = 1;
      move(m);
    }
  }
  if(!approve) LOG("Invalid move\n");
  return approve;
}


int main(){
  initGameLocal();
  char buf[256];
  int sendSize;
  initConnectionServ();
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  // the first client to connect gets to be white
  if(send(clientSockets[0], "i0", 2, 0) == -1){
    LOG("init message failed\n");
    perror("");
    exit(1);
  }
  if(send(clientSockets[1], "i1", 2, 0) == -1){
    LOG("init message failed\n");
    perror("");
    exit(1);
  }

  while(1){
    if(board.state != ONGOING){
      if(board.state == DRAW){
        sendC(clientSockets[0], "f2", 2, 0);
        sendC(clientSockets[1], "f2", 2, 0);
      }
      else{
        if(board.currentSide == 0){
          sendC(clientSockets[0], "f1", 2, 0);
          sendC(clientSockets[1], "f1", 2, 0);
        }
        else{
          sendC(clientSockets[0], "f0", 2, 0);
          sendC(clientSockets[1], "f0", 2, 0);
        }
      }
      break;
    }
    n = generateAllLegalMoves(mlist);
    recvC(clientSockets[board.currentSide], buf, 255, 0);
    if(interpretMsgServ(buf)){
      sendC(clientSockets[board.currentSide], buf, sizeof(struct msgM), 0);
      buf[0] = 'a';
      sendC(clientSockets[1-board.currentSide], buf, sizeof(struct msgM), 0);
    }
    else{
      buf[0] = 'd';
      sendSize = getCurrentFEN(buf+1);
      sendC(clientSockets[board.currentSide], buf, sendSize+1, 0);
    }
  }
  
  cleanGameData();
}
