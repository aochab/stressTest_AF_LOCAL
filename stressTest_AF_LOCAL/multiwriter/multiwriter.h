#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UNIX_PATH_MAX 108

int server_fd;
int client_fd;
int clientLOCAL_fd;
//int serverLocal_fd;

//FUNCTIONS
void createClientINET();
void communicationINET();
void createSerwerLOCAL(struct sockaddr_un *serverLOCALAdress, int *serverLocal_fd);
void acceptResponseLOCAL(int serverLocal_fd);
