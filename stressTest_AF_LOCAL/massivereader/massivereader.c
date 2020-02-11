#include "massivereader.h"

void getParameters(int argc, char* argv[])
{
    int opt;
    portNr = strtol(argv[1],NULL,10);
    
    while ((opt = getopt(argc, argv,"O:")) != -1)
    {
        switch(opt) 
        {
            case 'O':
            {
                int prefixLength = strlen(optarg);
                prefix = (char*)calloc(prefixLength,sizeof(char));
                strncpy(prefix,optarg,prefixLength);
                break;
            }
            default:
                printf("Wrong argument - exit program\n");
                exit(EXIT_FAILURE);
        }
    } 
}
//-----------------------------------------------------------------------------
void socketToNonblockingMode(int socked_fd)
{
	//Change server socket fd to nonblock mode
	int socketFlags = fcntl(socked_fd, F_GETFL,0);
	if( socketFlags == -1 ) 
	{
		perror("socketToNonblockingMode fcntl getflags");
		exit(EXIT_FAILURE);
	}
	if( fcntl(socked_fd,F_SETFL,socketFlags|O_NONBLOCK) == -1)
	{
		perror("socketToNonblockingMode fcntl setflags");
		exit(EXIT_FAILURE);
	}
}
//-----------------------------------------------------------------------
void createServerINET()
{
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( server_fd == -1 )
	{
	    perror("createServer Socket failed"); 
		exit(EXIT_FAILURE); 
	}
	
	struct sockaddr_in serverINETAddress;
    bzero(&serverINETAddress, sizeof(serverINETAddress));
	serverINETAddress.sin_family = AF_INET;
	serverINETAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverINETAddress.sin_port = htons(portNr);

	if(bind( server_fd, (struct sockaddr *)&serverINETAddress, sizeof(serverINETAddress)) < 0 ) 
	{
		perror("createServer bind failed"); 
		exit(EXIT_FAILURE);
	}	

	socketToNonblockingMode(server_fd);

    if (listen(server_fd, 5) < 0) 
	{ 
	    perror("createServer listen failed"); 
	    exit(EXIT_FAILURE); 
	} 
}
//------------------------------------------------------------------------
void acceptResponseINET()
{
	struct sockaddr_in clientINETAddress;
    int clientINETAddressLength = sizeof(clientINETAddress);

	if( (client_fd = accept( server_fd,(struct sockaddr *)&clientINETAddress,
			(socklen_t*)&clientINETAddressLength)) != -1)
	{
		socketToNonblockingMode(client_fd);
		//SET EPOLL
		struct epoll_event event;
		event.data.fd = client_fd;
		event.events = EPOLLIN | EPOLLET;
		if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&event)==-1)
		{
			perror("acceptResponseINET epoll_ctl");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if( (errno != EAGAIN) && (errno != EWOULDBLOCK))
		{
			perror("acceptResponseINET accept failed"); 
			exit(EXIT_FAILURE); 
		}
	}

}
//------------------------------------------------------------------------
int getResponseINET(struct sockaddr_un *clientLOCALAddress )
{ 
	while(1)
	{
		bzero((struct sockaddr_un *)clientLOCALAddress,sizeof(*clientLOCALAddress));
		//Przeczytaj struktture otrzymana od multiwriter, sprobuj sie polaczyc
        int bytesRead = read(client_fd,(struct sockaddr_un *)clientLOCALAddress, sizeof(*clientLOCALAddress));
        if( bytesRead != sizeof(struct sockaddr_un) )
        {
		//	perror("blad ");
			continue;
        }
		printf("CZYTAM %d\n",clientLOCALAddress->sun_family);
		break;
	}

}
//----------------------------------------------------------------------------
void createClientLOCAL(struct sockaddr_un *clientAddress, int clientLocal_fd)
{
	clientLocal_fd =  socket(AF_LOCAL, SOCK_STREAM, 0);
	if( clientLocal_fd == -1 )
	{
	    perror("createClientLOCAL Socket failed"); 
		exit(EXIT_FAILURE); 
	}
	if( connect( clientLocal_fd, (struct sockaddr *)clientAddress, sizeof(*clientAddress) ) < 0)
    {
        clientAddress->sun_family = -1;
		perror("createClientLOCAL Can't connect to local serwer");
    }
	else {
		printf("Connected to local server\n");
		localClientFds[numOfLocalClients] = clientLocal_fd;
		numOfLocalClients++;
	}
}
//---------------------------------------------------------------------------------
void sendInfoToINET(struct sockaddr_un clientLOCALAddress)
{
	//odsyla strukture do multiwriter&
	printf("Send %d \n",clientLOCALAddress.sun_family);
	if( write(client_fd, (struct sockaddr_un *)&clientLOCALAddress, sizeof(clientLOCALAddress)) == -1)
	{
		perror("createClientLOCAL write failed"); 
		exit(EXIT_FAILURE); 
	}
}
//--------------------------------------------------------------------------------
void communicationLOCAL(struct sockaddr_un clientAddress, int clientLocal_fd)
{
 //zrobis strukture ktora nam to odbierze te reprezentacje czasu bajtu i timesec
 char buff[255];
 bzero(buff,sizeof(buff));
    int bytesRead = read(clientLocal_fd,buff, sizeof(buff));
    if(bytesRead < 0)
    {
            perror("communicationLOCAL read failed");
    }
	printf("From local %s:",buff);
}
