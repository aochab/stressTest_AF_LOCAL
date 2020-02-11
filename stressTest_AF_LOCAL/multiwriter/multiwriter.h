#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UNIX_PATH_MAX 108
#define EVENTSMAX 50
#define LOCAL_SERVER_MAX 100

int portNr;
int numOfConnectionLOCAL;
float timeIntervalBeetwenMsg; //mikrosec 10^-6
float timeTotalWork; //centosekunda 10^-2

int client_fd;
int localsFds[LOCAL_SERVER_MAX];
int acceptedConnectionsLOCAL;
int receivedAnswersFromINET;
int numOfAcceptedConnectionINMultireader;

int epoll_fd;
struct epoll_event eventServer, eventClientINET, *events;

//FUNCTIONS
void getParameters(int argc, char* argv[]);
void socketToNonblockingMode(int socked_fd);
void createClientINET();
void getResponseFromINET();
void createSerwerLOCAL(struct sockaddr_un *mainServerLOCALAddress, int *mainServerLocal_fd);
void acceptResponseLOCAL(int serverLocal_fd, int *clientLOCAL_fd, struct sockaddr_un clientLOCALAddress);

void communicationLOCAL(int clientLOCAL_fd);