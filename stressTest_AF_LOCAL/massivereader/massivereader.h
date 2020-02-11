#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/un.h>

#define UNIX_PATH_MAX 108
#define EVENTSMAX 50
#define LOCAL_CLIENT_MAX 100

int portNr;
char* prefix;

int server_fd;
int client_fd;

int localClientFds[LOCAL_CLIENT_MAX];
struct sockaddr_un localClientsAdresses[LOCAL_CLIENT_MAX];
int numOfLocalClients;

int epoll_fd;
struct epoll_event eventServer, *events;

typedef struct message {
    char textTime[19];
    char socketPath[108];
    struct timespec time;
} Message;

//FUNCTIONS
void getParameters(int argc, char* argv[]);
void socketToNonblockingMode(int socked_fd);

void createServerINET();
void acceptResponseINET();
int getResponseINET(struct sockaddr_un *clientLOCALAdress );

void createClientLOCAL(struct sockaddr_un *clientAddress, int *clientLocal_fd);
void sendInfoToINET(struct sockaddr_un clientLOCALAddress);
void communicationLOCAL(struct sockaddr_un clientAddress, int clientLocal_fd);