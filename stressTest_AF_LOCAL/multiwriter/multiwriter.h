#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


int server_fd;
int client_fd;

//FUNCTIONS
void createClientINET();
void communicationINET(int socket_fd);
void createSerwerLOCAL();
