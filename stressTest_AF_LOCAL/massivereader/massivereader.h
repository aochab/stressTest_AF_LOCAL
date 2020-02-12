#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define UNIX_PATH_MAX 108
#define EVENTSMAX 50
#define LOCAL_CLIENT_MAX 100
#define TEXT_TIME_REPRESENATION 25

int portNr;
char* prefix;

int fileToWriteResultsNumber;
int fileToWriteDescriptor;
int oldFileToWriteDescriptor;

int server_fd;
int client_fd;

int localClientFds[LOCAL_CLIENT_MAX];
struct sockaddr_un localClientsAdresses[LOCAL_CLIENT_MAX];
int numOfLocalClients;

int epoll_fd;
struct epoll_event eventServer, *events;

typedef struct message {
    char textTime[TEXT_TIME_REPRESENATION];
    char socketPath[UNIX_PATH_MAX];
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
int communicationLOCAL(struct sockaddr_un clientAddress, int clientLocal_fd);

void makeTextualRepresentationOfTime(char* textTime, struct timespec timeStruct);
struct timespec timeDifference(struct timespec timeStart, struct timespec timeStop);

int createFile();

//SIGNAL
void setSignalHandlerSIGUSR1CreateFile();
void signalHandlerSIGUSR1CreateFile(int sig);

void exitFunction(void);