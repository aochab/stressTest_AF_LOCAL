#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>

#include <sys/epoll.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define UNIX_PATH_MAX 108
#define EVENTSMAX 50
#define LOCAL_SERVER_MAX 100
#define TEXT_TIME_REPRESENATION 25

int portNr;
int numOfConnectionLOCAL;
float timeIntervalBeetwenMsg; //mikrosec 10^-6
float timeTotalWork; //centosekunda 10^-2

struct timespec timeIntervalBeetwenMsgConverted;
struct timespec timeTotalWorkConvertConverted;

int client_fd;
int localsFds[LOCAL_SERVER_MAX];
int acceptedConnectionsLOCAL;
int receivedAnswersFromINET;
int numOfAcceptedConnectionINMultireader;

int epoll_fd;
struct epoll_event eventServer, eventClientINET, *events;

typedef struct message {
    char textTime[TEXT_TIME_REPRESENATION];
    char socketPath[UNIX_PATH_MAX];
    struct timespec time;
} Message;

struct timespec startProgramTime;
struct timespec startSendMessagesTime;
struct timespec stopSendMessagesTime;
struct timespec totalSendMsgTime;
struct timespec minSendMsgTime;
struct timespec maxSendMsgTime;

//FUNCTIONS
void getParameters(int argc, char* argv[]);
void socketToNonblockingMode(int socked_fd);
void createClientINET();
void getResponseFromINET();
void createSerwerLOCAL(struct sockaddr_un *mainServerLOCALAddress, int *mainServerLocal_fd);
void acceptResponseLOCAL(int serverLocal_fd, int *clientLOCAL_fd, struct sockaddr_un clientLOCALAddress);

void changeUnitsMicrosecToSecAndNsec(float inputMicroSec, long *outSec, long *outNsec);
void changeUnitsCentisecToSecAndNsec(float inputCentiSec, long *outSec, long *outNsec);

void sendMessage(struct sockaddr_un mainServerLOCALAddress);
void makeTextualRepresentationOfTime(char* textTime, struct timespec timeStruct);
struct timespec timeDifference(struct timespec timeStart, struct timespec timeStop);
void makeInfoAboutSendingMessagesTime(struct timespec sendMsgTime);

void setTimer();
void signalHandler();
void exitFunction(void);