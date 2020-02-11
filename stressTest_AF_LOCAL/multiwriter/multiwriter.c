#include "multiwriter.h"

void getParameters(int argc, char* argv[])
{
    int opt;
    
    while ((opt = getopt(argc, argv,"S:p:d:T:")) != -1)
    {
        switch(opt) 
        {
			case 'S':
            {
                numOfConnectionLOCAL = strtol(optarg,NULL,10);
                break;
            }
            case 'p':
            {
                portNr = strtol(optarg,NULL,10);
                break;
            }
			case 'd':
            {
                timeIntervalBeetwenMsg = strtof(optarg,NULL);
                break;
            }
			case 'T':
            {
                timeTotalWork = strtof(optarg,NULL);
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
//--------------------------------------------------------
void createClientINET()
{
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( client_fd == -1 )
	{
	    perror("createClientINET Socket failed"); 
		exit(EXIT_FAILURE); 
	}

    struct sockaddr_in clientINETAddress;
    bzero(&clientINETAddress, sizeof(clientINETAddress));
	clientINETAddress.sin_family = AF_INET;
	clientINETAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientINETAddress.sin_port = htons(portNr);

    if( connect( client_fd, (struct sockaddr *)&clientINETAddress, sizeof(clientINETAddress) ) < 0)
    {
        perror("createClientINET connect failed"); 
		exit(EXIT_FAILURE);
    }
	
}
//------------------------------------------------------------------------------
void getResponseFromINET()
{
	struct sockaddr_un response;

	memset(&response,0, sizeof(struct sockaddr_un));
	if(read(client_fd, (struct sockaddr_un *)&response, sizeof(response)) != sizeof(struct sockaddr_un))
	{
		perror("getResponseFromINET failed");
	}
	else 
	{
		receivedAnswersFromINET++;
		if( response.sun_family == AF_LOCAL ) 
		{
			numOfAcceptedConnectionINMultireader++;
		}
		printf("From serwer : %d\n",response.sun_family);
	}
	
}
//-------------------------------------------------------------------------------
void createSerwerLOCAL(struct sockaddr_un *mainServerLOCALAddress, int *mainServerLocal_fd)
{//POPRAWNE
	//Linux Abstract socket namespace
	memset(mainServerLOCALAddress,0, sizeof(struct sockaddr_un));
	mainServerLOCALAddress->sun_family = AF_LOCAL;
	//Create random adress
	int pathLength = sizeof(*mainServerLOCALAddress)-2;
	char* address = (char*)calloc(pathLength,sizeof(char));
	for(int i=0; i<pathLength; i++)
	{
		address[i] = rand();
	}
	strncpy(&mainServerLOCALAddress->sun_path[1],address,pathLength);

	*mainServerLocal_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if( *mainServerLocal_fd == -1 )
	{
	    perror("createSerwerLOCAL Socket failed");
		exit(EXIT_FAILURE); 
	}

	if(bind( *mainServerLocal_fd, (struct sockaddr *)mainServerLOCALAddress, sizeof(*mainServerLOCALAddress)) < 0 ) 
	{
		perror("createSerwerLOCAL bind failed"); 
		exit(EXIT_FAILURE);
	}

	socketToNonblockingMode(*mainServerLocal_fd);

	if (listen(*mainServerLocal_fd, 5) < 0) 
	{ 
	    perror("createServer listen failed"); 
	    exit(EXIT_FAILURE); 
	} 
}
//-----------------------------------------------------------------------------------
void acceptResponseLOCAL(int serverLocal_fd, int *clientLOCAL_fd, struct sockaddr_un clientLOCALAddress)
{
    int clientLOCALAddressLength = sizeof(clientLOCALAddress);

	if( (*clientLOCAL_fd = accept(serverLocal_fd,(struct sockaddr *)&clientLOCALAddress,
			(socklen_t*)&clientLOCALAddressLength)) != -1)
	{
		socketToNonblockingMode(*clientLOCAL_fd);
	/*	struct epoll_event event;
		event.data.fd = *clientLOCAL_fd;
		event.events = EPOLLIN | EPOLLET;
		if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,*clientLOCAL_fd,&event)==-1)
		{
			perror("acceptResponseINET epoll_ctl");
			exit(EXIT_FAILURE);
		}*/
		//Save witch connections are succesfull
		localsFds[acceptedConnectionsLOCAL]=*clientLOCAL_fd;
		acceptedConnectionsLOCAL++;
	}
	else
	{
		if( (errno != EAGAIN) && (errno != EWOULDBLOCK))
		{
			perror("createResponseLOCAL  accept failed"); 
			exit(EXIT_FAILURE); 
		}
	}
}
//---------------------------------------------------------------------------------
void communicationLOCAL(int clientLOCAL_fd)
{
	char buff[255];
	int n;

		bzero(buff,sizeof(buff));
		n=0;
		while((buff[n++] = getchar()) != '\n')
			;
		
		write(clientLOCAL_fd, buff, sizeof(buff));
}
