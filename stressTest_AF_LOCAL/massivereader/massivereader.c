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
/*
	//Change server socket fd to nonblock mode
	int serverFlags = fcntl(server_fd, F_GETFL,0);
	if( serverFlags == -1 ) 
	{
		perror("createServerINET fcntl getflags");
		exit(EXIT_FAILURE);
	}
	if( fcntl(server_fd,F_SETFL,serverFlags|O_NONBLOCK) == -1)
	{
		perror("createServerINET fcntl setflags");
		exit(EXIT_FAILURE);
	}
*/
    if (listen(server_fd, 5) < 0) 
	{ 
	    perror("createServer listen failed"); 
	    exit(EXIT_FAILURE); 
	} 
}

void createClientResponseINET()
{
	struct sockaddr_in clientINETAddress;
//	struct sockaddr_un streamData;
    int clientINETAddressLength = sizeof(clientINETAddress);

	client_fd = accept(
			server_fd,
			(struct sockaddr *)&clientINETAddress,
			(socklen_t*)&clientINETAddressLength);

	if(client_fd<0)
	{
		perror("createClientINET accept failed"); 
		exit(EXIT_FAILURE); 
	}
/*	
	//Change client socket fd to nonblock mode
	int clientFlags = fcntl(client_fd, F_GETFL,0);
	if( clientFlags == -1 ) 
	{
		perror("createClientINET fcntl getflags");
		exit(EXIT_FAILURE);
	}
	if( fcntl(client_fd,F_SETFL,clientFlags|O_NONBLOCK) == -1)
	{
		perror("createClientINET fcntl setflags");
		exit(EXIT_FAILURE);
	}

	//EPOLL set
	struct epoll_event event;
	event.data.fd = client_fd;
	event.events = EPOLLIN | EPOLLET;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
	{
		perror("createClientINET epoll_ctl");
		exit(EXIT_FAILURE);
	}*/
}
//------------------------------------------------------------------------
void communicationINET(int socket_fd)
{
	/*struct sockaddr_un streamData;

	while(1)
	{
        int bytesRead = read(client_fd, streamData, sizeof(streamData));
        if(bytesRead != sizeof(streamData))
        {
            streamData.sun_family = -1;
        }
		printf("From client %s\n",streamData);*/
//		send(client_fd, streamData, sizeof(streamData),0);
//		printf("Message sent\n");
//	}
	char buff[255];
	int n;
	while(1)
	{
		bzero(buff,sizeof(buff));
		read(socket_fd, buff, sizeof(buff));

		printf("From client : %s \t to client :",buff);
		bzero(buff,sizeof(buff));
		n=0;
		while((buff[n++] = getchar()) != '\n')
			;
		
		write(socket_fd, buff, sizeof(buff));

		if(strncmp("exit",buff,4) == 0 ) 
		{
			printf("Server exit\n");
			break;
		}
	}
}