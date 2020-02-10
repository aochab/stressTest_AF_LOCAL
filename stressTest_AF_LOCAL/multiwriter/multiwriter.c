#include "multiwriter.h"

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
	clientINETAddress.sin_port = htons(12345);

    if( connect( client_fd, (struct sockaddr *)&clientINETAddress, sizeof(clientINETAddress) ) < 0)
    {
        perror("createClientINET connect failed"); 
		exit(EXIT_FAILURE);
    }
	
}
//------------------------------------------------------------------------------
void communicationINET(int socket_fd)
{
	struct sockaddr_un serverLOCALAdress;
	//int serverLocal_fd;

	//while(1)
//	{
		createSerwerLOCAL(&serverLOCALAdress,&serverLocal_fd);
	//	bzero((struct sockaddr_un *)&clientLOCALAdress,sizeof(clientLOCALAdress));
	//	clientLOCALAdress.sun_family = 123;
	//	write(socket_fd, (struct sockaddr_un *)&clientLOCALAdress, sizeof(clientLOCALAdress));
	//}

/*
	char buff[255];
	int n;
	while(1)
	{
		bzero(buff,sizeof(buff));
		printf("Enter string : ");
        n=0;
		while((buff[n++] = getchar()) != '\n')
			;
		
		write(socket_fd, buff, sizeof(buff));
        bzero( buff, sizeof(buff));
        read(socket_fd, buff, sizeof(buff));
        printf("From server : %s",buff);
		if(strncmp("exit",buff,4) == 0 ) 
		{
			printf("Server exit\n");
			break;
		}
	}*/
}
//-------------------------------------------------------------------------------
void createSerwerLOCAL(struct sockaddr_un *serverLOCALAdress, int* serverLocal_fd)
{
	//Linux Abstract socket namespace
	memset(serverLOCALAdress,0, sizeof(struct sockaddr_un));
	serverLOCALAdress->sun_family = AF_LOCAL;
	strncpy(&serverLOCALAdress->sun_path[1],"xyz",sizeof(serverLOCALAdress->sun_path)-1);

	*serverLocal_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if( *serverLocal_fd == -1 )
	{
	    perror("createSerwerLOCAL Socket failed");
		exit(EXIT_FAILURE); 
	}

	if(bind( *serverLocal_fd, (struct sockaddr *)serverLOCALAdress, sizeof(serverLOCALAdress)) < 0 ) 
	{
		perror("createSerwerLOCAL bind failed"); 
		exit(EXIT_FAILURE);
	}	

	if (listen(*serverLocal_fd, 5) < 0) 
	{ 
	    perror("createServer listen failed"); 
	    exit(EXIT_FAILURE); 
	} 

	createResponseLOCAL();

	char buff[255];
	int n;
	while(1)
	{
		bzero(buff,sizeof(buff));
		read(*serverLocal_fd, buff, sizeof(buff));

		printf("From client : %s \t to client :",buff);
		bzero(buff,sizeof(buff));
		n=0;
		while((buff[n++] = getchar()) != '\n')
			;
		
		write(*serverLocal_fd, buff, sizeof(buff));

		if(strncmp("exit",buff,4) == 0 ) 
		{
			printf("Server exit\n");
			break;
		}
	}
}
//-----------------------------------------------------------------------------------
void createResponseLOCAL()
{
	struct sockaddr_in clientLOCALAddress;
    int clientLOCALAddressLength = sizeof(clientLOCALAddress);

	clientLOCAL_fd = accept(
			serverLocal_fd,
			(struct sockaddr *)&clientLOCALAddress,
			(socklen_t*)&clientLOCALAddressLength);

	if(clientLOCAL_fd<0)
	{
		perror("createResponseLOCAL accept failed"); 
		exit(EXIT_FAILURE); 
	}
}