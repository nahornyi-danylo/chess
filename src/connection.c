#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#define LOG_PREFIX "connection"
#include "../include/common.h"
#include "../include/connection.h"

int recvC(int socket, void *msg, int size, int flags){
  int ret = recv(socket, msg, size, flags);
  DEBUG_LOG("Received a message\n");
  if(ret == -1){
    LOG("recv failed\n");
    perror("");
    exit(1);
  }
  if(ret == 0){
    LOG("Connection closed\n");
  }
  return ret;
}

void sendC(int socket, void *msg, int size, int flags){
  int ret;
  char *buf = (char *)msg;
  int total = 0;

  while(total < size){
    ret = send(socket, buf + total, size - total, flags);

    if(ret < 0){
      LOG("send failed\n");
      perror("");
      exit(1);
    }

    if (ret == 0) {
      LOG("Connection closed");
      exit(1);
    }

    total += ret;
  }

  DEBUG_LOG("Sent a message\n");
}
