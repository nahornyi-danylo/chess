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
  return ret;
}

void sendC(int socket, void *msg, int size, int flags){
  int ret = send(socket, msg, size, flags);
  DEBUG_LOG("Sent a message\n");
  if(ret == -1){
    LOG("recv failed\n");
    perror("");
    exit(1);
  }
  if(ret < size){
    LOG("full message not sent\n");
    exit(1);
  }
}
