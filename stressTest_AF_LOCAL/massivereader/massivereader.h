#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/un.h>

#define UNIX_PATH_MAX 108

int portNr;
char* prefix;

int server_fd;
int client_fd;
int epoll_fd;

//FUNCTIONS
void getParameters(int argc, char* argv[]);

void createServerINET();
void acceptResponseINET();
void communicationINET(struct sockaddr_un *clientLOCALAdress, int *clientLocal_fd );

void createClientLOCAL(struct sockaddr_un *clientAddress, int *clientLocal_fd);
void communicationLOCAL(struct sockaddr_un clientAddress, int clientLocal_fd);