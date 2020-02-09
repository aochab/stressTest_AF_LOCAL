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


int portNr;
char* prefix;

int server_fd;
int client_fd;
int epoll_fd;

//FUNCTIONS
void getParameters(int argc, char* argv[]);
void createServerINET();
void createClientResponseINET();
void communicationINET(int socket_fd);