#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#define LOG_PREFIX "client"
#include "../include/common.h"
#include "../include/chess.h"
#include "../include/chessUI.h"
#include "../include/game.h"
#include "../include/connection.h"

extern struct board board;
extern pthread_mutex_t mutex;
extern struct drawInfo boardInfo;
int serverSocket;
char buf[256];

int initConnection(const char *ip, const char *port){
  struct addrinfo hints = {0};
  struct addrinfo *servinfo, *p;
  int error;
  char s[INET_ADDRSTRLEN];

  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if((error = getaddrinfo(ip, port, &hints, &servinfo)) != 0){
    LOG("getaddrinfo: %s\n", gai_strerror(error));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next){
    if((serverSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      LOG("socket error:\n");
      perror("");
      continue;
    }

    if(connect(serverSocket, p->ai_addr, p->ai_addrlen) == -1){
      LOG("connection error:\n");
      perror("");
      close(serverSocket);
      continue;
    }

    break;    
  }

  if (p == NULL) {
    LOG("failed to connect\n");
    freeaddrinfo(servinfo);
    return 1;
  }

  inet_ntop(p->ai_family,
            &((struct sockaddr_in *)p->ai_addr)->sin_addr,
            s, sizeof(s));
  LOG("connected to %s:%s\n", s, port);

  freeaddrinfo(servinfo);

  // receive the init msg
  recvC(serverSocket, buf, 255, 0);
  if(buf[0] == 'i'){
    *boardInfo.playingAs = buf[1]-'0';
  }
  return 0;
}

void interpretMsg(){
  struct move m;
  if(buf[0] == 'd'){
    LOG("move disapproved, reloading FEN\n");
    loadFEN(((char *)buf)+1);
  }
  else if(buf[0] == 'a'){
    m = *(struct move *)(buf+1);
    LOG("move approved\n");
    makeMoveReceive(&m);
  }
  else if(buf[0] == 'm'){
    m = *(struct move *)(buf+1);
    LOG("received a move\n");
    makeMoveReceive(&m);
  }
  else if(buf[0] == 'f'){
    LOG("Game finished!\n");
    switch(buf[1]){
      case '0':
        LOG("White won!\n");
        break;
      case '1':
        LOG("Black won!\n");
        break;
      case '2':
        LOG("Draw!\n");
        break;
    }
  }
  else{
    LOG("received junk\n");
  }
}

void *clientConnectionThread(void *ptr){
  int n;
  while(1){
    n = recvC(serverSocket, buf, 255, 0);
    if(n>0){
      interpretMsg();
    }
    else {
      LOG("connetion terminated\n");
      break;
    }
  }
  return NULL;
}





