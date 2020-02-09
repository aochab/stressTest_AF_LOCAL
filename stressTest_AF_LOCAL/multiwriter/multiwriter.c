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
	}
}
//-------------------------------------------------------------------------------
void createSerwerLOCAL();