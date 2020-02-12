#include "massivereader.h"

int main(int argc, char* argv[])
{
    if(argc<2)
    {
        printf("Too few arguments\nRemember about give port number as first argument\n");
        exit(EXIT_FAILURE);
    }
    portNr=0;
    if(atexit(exitFunction) != 0 )
    {
        perror("ATEXIT error");
        exit(EXIT_FAILURE);
    }

    getParameters(argc,argv);
    numOfLocalClients=0;
    fileToWriteResultsNumber=0;

    if( createFile() == -1 )
    {
        printf("Can't create and find file with read permission.\nChange prefix\n");
        exit(EXIT_FAILURE);
    }

    setSignalHandlerSIGUSR1CreateFile();


    printf("Waiting fom INET client...\n\n");
    createServerINET();

    //Time delay beetwen creating LOCAL clients
    struct timespec time; 
    time.tv_sec=0;
    time.tv_nsec=30000000;

    epoll_fd = epoll_create1(0);
    if( epoll_fd == -1 )
    {
        perror("Epoll_create1 main");
        exit(EXIT_FAILURE);
    }
    
    eventServer.data.fd = server_fd;
    eventServer.events = EPOLLIN | EPOLLET;
    if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &eventServer) == -1)
    {
        perror("Epoll_ctl main");
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
           
            if (events[i].data.fd == server_fd)
            {
                //Set new socket
                printf("Accept response inet\n");
                acceptResponseINET();
            }
            else
            {
                //new LOCAL Socket
                while(1)
                {
                    struct sockaddr_un clientLOCALAddress;
                    int clientLocal_fd;
                    if( getResponseINET(&clientLOCALAddress) == -1)
                    {
                        close(server_fd);
                        break;
                    } 
                    createClientLOCAL(&clientLOCALAddress,&clientLocal_fd);
                    sendInfoToINET(clientLOCALAddress);
                    nanosleep(&time,0);
                }
            }

            for(int j=0; j<numOfLocalClients; j++)
            {
                if (events[i].data.fd == localClientFds[j])
                {
                    if(communicationLOCAL(localClientsAdresses[j],localClientFds[j]) == -1)
                        break;
                }
            }
        }
        if(numReady == numOfLocalClients) break;
    }

    exit(EXIT_SUCCESS);
}