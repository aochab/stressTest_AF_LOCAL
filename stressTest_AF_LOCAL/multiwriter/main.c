#include "multiwriter.h"

int main(int argc, char* argv[])
{
    if(argc<9)
    {
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    struct timespec time; time.tv_nsec=0; time.tv_sec=2;
    getParameters(argc,argv);
    acceptedConnectionsLOCAL = 0;
    receivedConnectionsLOCAL = 0;
    //Create main socket Local stream
    struct sockaddr_un mainServerLOCALAddress;
    int mainServerLocal_fd;
    int mainClientLocal_fd;

    createSerwerLOCAL(&mainServerLOCALAddress,&mainServerLocal_fd);

    createClientINET();

    for(int i=0;i<numOfConnectionLOCAL;i++)
	{
		write(client_fd, (struct sockaddr_un *)&mainServerLOCALAddress, sizeof(mainServerLOCALAddress));
	}
    

    
   // nanosleep(&time,0);
    //communicationINET(&mainServerLocal_fd,&mainClientLocal_fd);
   // communicationLOCAL(mainClientLocal_fd);
    
    //EPOLL
    
    epoll_fd = epoll_create1(0);
    if( epoll_fd == -1 )
    {
        perror("Epoll_create1 main");
        exit(EXIT_FAILURE);
    }

    //epoll for LOCAL
    eventServer.data.fd = mainServerLocal_fd;
    eventServer.events = EPOLLIN | EPOLLET ;
    if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, mainServerLocal_fd, &eventServer) == -1)
    {
        perror("Epoll_ctl main");
        exit(EXIT_FAILURE);
    }

    events = calloc(EVENTSMAX, sizeof(struct epoll_event));
    if( events == NULL) 
    {
        perror("Calloc events error main");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        int numReady = epoll_wait(epoll_fd, events, EVENTSMAX, -1);
        for( int i=0; i < numReady; i++)
        {
            if( events[i].events & EPOLLERR || 
                events[i].events & EPOLLHUP || 
                !(events[i].events & EPOLLIN))
            {
                perror("Epoll main error");
                close(events[i].data.fd);
            }
            if (events[i].data.fd == mainServerLocal_fd)
            {
                //Set new socket
                int clientLocal_fd;
                acceptResponseLOCAL(mainServerLocal_fd,&clientLocal_fd,mainServerLOCALAddress);
                printf("akceptuje local\n");
                printf("rece %d num of con %d",receivedConnectionsLOCAL,numOfConnectionLOCAL);
                if(receivedConnectionsLOCAL == (numOfConnectionLOCAL-1) )
                {
                    close(client_fd);
                }
            }
            else
            {
                printf("gotowy local\n");
            }
        }
    }


    
    close(client_fd);
 
    exit(EXIT_SUCCESS);
}
