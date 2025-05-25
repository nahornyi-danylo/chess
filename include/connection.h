
int initConnection(const char *ip, const char *port);
int recvC(int socket, void *msg, int size, int flags);
void sendC(int socket, void *msg, int size, int flags);
void *clientConnectionThread(void *ptr);
