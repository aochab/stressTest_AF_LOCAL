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
void communicationINET(int *serverLocal_fd, int *clientLocal_fd)
{
	struct sockaddr_un serverLOCALAdress;
	struct sockaddr_un response;
	//while(1)
	//{
	createSerwerLOCAL(&serverLOCALAdress,serverLocal_fd);
	write(client_fd, (struct sockaddr_un *)&serverLOCALAdress, sizeof(serverLOCALAdress));
	acceptResponseLOCAL(*serverLocal_fd,clientLocal_fd);

	memset(&response,0, sizeof(struct sockaddr_un));
	read(client_fd, (struct sockaddr_un *)&response, sizeof(response));
	printf("From serwer : %d\n",response.sun_family);
		
	//}

}
//-------------------------------------------------------------------------------
void createSerwerLOCAL(struct sockaddr_un *serverLOCALAdress, int* serverLocal_fd)
{
	//Linux Abstract socket namespace
	memset(serverLOCALAdress,0, sizeof(struct sockaddr_un));
	serverLOCALAdress->sun_family = AF_LOCAL;
	strncpy(&serverLOCALAdress->sun_path[1],"xyz",sizeof(serverLOCALAdress->sun_path)-2);

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
}
//-----------------------------------------------------------------------------------
void acceptResponseLOCAL(int serverLocal_fd, int *clientLOCAL_fd)
{
	struct sockaddr_in clientLOCALAddress;
    int clientLOCALAddressLength = sizeof(clientLOCALAddress);

	*clientLOCAL_fd = accept(
			serverLocal_fd,
			(struct sockaddr *)&clientLOCALAddress,
			(socklen_t*)&clientLOCALAddressLength);

	if(*clientLOCAL_fd<0)
	{
		perror("createResponseLOCAL accept failed"); 
		exit(EXIT_FAILURE); 
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