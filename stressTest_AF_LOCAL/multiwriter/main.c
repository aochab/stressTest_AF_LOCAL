#include "multiwriter.h"

int main(int argc, char* argv[])
{
    if(argc<9)
    {
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));

    if(atexit(exitFunction) != 0 )
    {
        perror("ATEXIT error");
        exit(EXIT_FAILURE);
    }
    
    getParameters(argc,argv);

    //Convert units from parameters
    changeUnitsMicrosecToSecAndNsec(timeIntervalBeetwenMsg,
                                    &timeIntervalBeetwenMsgConverted.tv_sec,
                                    &timeIntervalBeetwenMsgConverted.tv_nsec);
    printf("Beetween msg sec %ld nsec %ld\n",timeIntervalBeetwenMsgConverted.tv_sec,
                                            timeIntervalBeetwenMsgConverted.tv_nsec);

    changeUnitsCentisecToSecAndNsec(timeTotalWork,
                                    &timeTotalWorkConvertConverted.tv_sec,
                                    &timeTotalWorkConvertConverted.tv_nsec);
    printf("Total work sec %ld nsec %ld\n",timeTotalWorkConvertConverted.tv_sec,
                                            timeTotalWorkConvertConverted.tv_nsec);

    
    acceptedConnectionsLOCAL = 0;
    receivedAnswersFromINET = 0;
    numOfAcceptedConnectionINMultireader = 0;
    //Create main socket Local stream
    struct sockaddr_un mainServerLOCALAddress;
    int mainServerLocal_fd;

    createSerwerLOCAL(&mainServerLOCALAddress,&mainServerLocal_fd);

    createClientINET();

    for(int i=0;i<numOfConnectionLOCAL;i++)
	{
		write(client_fd, (struct sockaddr_un *)&mainServerLOCALAddress, sizeof(mainServerLOCALAddress));
	}

    
    //EPOLL
    
    epoll_fd = epoll_create1(0);
    if( epoll_fd == -1 )
    {
        perror("Epoll_create1 main");
        exit(EXIT_FAILURE);
    }

    //epoll for INET
    eventClientINET.data.fd = client_fd;
    eventClientINET.events = EPOLLIN | EPOLLET;
    if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &eventClientINET) == -1)
    {
        perror("Epoll_ctl eventClient INET main");
        exit(EXIT_FAILURE);
    }

    //epoll for LOCAL
    eventServer.data.fd = mainServerLocal_fd;
    eventServer.events = EPOLLIN | EPOLLET ;
    if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, mainServerLocal_fd, &eventServer) == -1)
    {
        perror("Epoll_ctl eventServer LOCAL main");
        exit(EXIT_FAILURE);
    }

    events = calloc(EVENTSMAX, sizeof(events));
    if( events == NULL) 
    {
        perror("Calloc events error main");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        printf("ODEBRANO %d\n",receivedAnswersFromINET);
        if(receivedAnswersFromINET == numOfConnectionLOCAL)
        {
            close(client_fd);
            close(mainServerLocal_fd);
            break;
        }
        int numReady = epoll_wait(epoll_fd, events, EVENTSMAX, -1);
        if(numReady == -1)
        {
            perror("Epoll_wait main");
            exit(EXIT_FAILURE);
        }

        for( int i=0; i < numReady; i++)
        {
            if( events[i].events & EPOLLERR || 
                events[i].events & EPOLLHUP || 
                !(events[i].events & EPOLLIN))
            {
                perror("Epoll main");
                close(events[i].data.fd);
            }
            //Accept connection to LOCAL
            if (events[i].data.fd == mainServerLocal_fd)
            {
                //Set new socket
                int clientLocal_fd;
                acceptResponseLOCAL(mainServerLocal_fd,&clientLocal_fd,mainServerLOCALAddress);
                printf("Accept response from multireader - connected to LOCAl\n");
            }

            if (events[i].data.fd == client_fd)
            {
                //INET respones about connection with local
                printf("Get response from INET\n");
                getResponseFromINET();
            }  
        }
    }
    printf("\n\n===================== SEND MESSAGES =====================\n\n");

    setTimer();
    
    if(clock_gettime(CLOCK_REALTIME,&startSendMessagesTime) == -1) 
    {
        perror("clock_getime startSendMessagesTime");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        sendMessage(mainServerLOCALAddress);
    }

//TODO Monitorowac ktore sockety zostaly zamkniete w trakcie 
    exit(EXIT_SUCCESS);
}
