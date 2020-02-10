#include "massivereader.h"

int main(int argc, char* argv[])
{
    if(argc<3)
    {
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }

    getParameters(argc,argv);
    createServerINET();

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
            else if (events[i].data.fd == server_fd)
            {
                //Set new socket
                printf("Accept response inet\n");
                acceptResponseINET();
            }
            else
            {
                //Socket is ready
                //new LOCAL Socket
                struct sockaddr_un clientLOCALAddress;
                int clientLocal_fd;
                printf("Communication inet\n");
                communicationINET(&clientLOCALAddress);
                printf("ATUTAJ %d",clientLOCALAddress.sun_family);
                createClientLOCAL(clientLOCALAddress,clientLocal_fd);
            //    communicationLOCAL(clientLOCALAdress,clientLocal_fd);
/*
                    bzero((struct sockaddr_un *)&clientLOCALAddress,sizeof(clientLOCALAddress));
                //Przeczytaj struktture otrzymana od multiwriter, sprobuj sie polaczyc
                int bytesRead = read(client_fd,(struct sockaddr_un *)&clientLOCALAddress, sizeof(clientLOCALAddress));
                if(bytesRead < 0)
                {
                    perror("communicationINET read failed");
                }
                printf("CZYTAM %d\n",clientLOCALAddress.sun_family);*/
            }
            
        }
    }



 ///   printf("Port %d prefix %s",portNr,prefix);
    close(server_fd);
    close(client_fd);
    free(prefix);
    exit(EXIT_SUCCESS);
}