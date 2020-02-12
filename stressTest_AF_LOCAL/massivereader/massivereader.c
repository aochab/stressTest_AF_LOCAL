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
			return -1;
        }
		break;
	}
	return 0;
}
//----------------------------------------------------------------------------
void createClientLOCAL(struct sockaddr_un *clientAddress, int *clientLocal_fd)
{
	*clientLocal_fd =  socket(AF_LOCAL, SOCK_STREAM, 0);
	if( *clientLocal_fd == -1 )
	{
	    perror("createClientLOCAL Socket failed"); 
		exit(EXIT_FAILURE); 
	}
	if( connect( *clientLocal_fd, (struct sockaddr *)clientAddress, sizeof(*clientAddress) ) < 0)
    {
        clientAddress->sun_family = -1;
		perror("createClientLOCAL Can't connect to local serwer");
    }
	else {
		printf("Connected to local server\n");
		localClientFds[numOfLocalClients] = *clientLocal_fd;
		localClientsAdresses[numOfLocalClients] = *clientAddress;
		numOfLocalClients++;

		//SET EPOLL
		socketToNonblockingMode(*clientLocal_fd);
		struct epoll_event event;
		event.data.fd = *clientLocal_fd;
		event.events = EPOLLIN | EPOLLET;
		if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,*clientLocal_fd,&event)==-1)
		{
			perror("createClientLOCAL epoll_ctl");
			exit(EXIT_FAILURE);
		}
	}
}
//---------------------------------------------------------------------------------
void sendInfoToINET(struct sockaddr_un clientLOCALAddress)
{
	//odsyla strukture do multiwriter
	printf("Send %d \n",clientLOCALAddress.sun_family);
	if( write(client_fd, (struct sockaddr_un *)&clientLOCALAddress, sizeof(clientLOCALAddress)) == -1)
	{
		if( errno != ECONNRESET )
		{
			perror("createClientLOCAL write failed "); 
			exit(EXIT_FAILURE); 
		}	
	}
}
//--------------------------------------------------------------------------------
void communicationLOCAL(struct sockaddr_un clientAddress, int clientLocal_fd)
{
	Message msg;
	int bytesRead = read(clientLocal_fd,&msg,sizeof(Message));
	if(bytesRead != sizeof(Message))
	{
		perror("Read from local error");
	}

	//Get Wall clock
	struct timespec msgCommingTime;
	if(clock_gettime(CLOCK_REALTIME,&msgCommingTime) == -1) 
    {
        perror("clock_getime stopSendMessagesTime");
        exit(EXIT_FAILURE);
    }

	struct timespec delayTime = timeDifference(msg.time,msgCommingTime);

	//Verify path sender
	if(!strcmp(clientAddress.sun_path,msg.socketPath)) 
	{
		printf("Socket verified correctly\n");
	}
	else 
	{ 
		printf("Socket verified negative\n"); 
	}

	char* textDelayTime = (char*)calloc(TEXT_TIME_REPRESENATION+1,sizeof(char));
	makeTextualRepresentationOfTime(textDelayTime,delayTime);

    printf("Send time %s Delay time %s\n",msg.textTime,textDelayTime);
	free(textDelayTime);
}
//----------------------------------------------------------------------------
void makeTextualRepresentationOfTime(char* textTime, struct timespec timeStruct)
{
    long nSec = timeStruct.tv_nsec;
    int sec;
    int minutes;
    if(timeStruct.tv_sec<60)
    {
        sec = timeStruct.tv_sec;
        minutes = 0;
    }
    else
    {
        sec = timeStruct.tv_sec%60; 
        minutes = (timeStruct.tv_sec%3600)/60;
    }

    if(minutes>99)
    {
        exit(EXIT_FAILURE);
    }
    if(minutes>9)
    {
        textTime[0]=(char)48;
        textTime[1]=(char)(minutes/10) + (char)48;
        textTime[2]=(char)(minutes%10) + (char)48;
    }
    else
    {
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)(minutes%10)+ (char)48;
    }
    textTime[3]='*'; textTime[4]=':';
    if(sec>9)
    {
        textTime[5]=(char)(sec/10) + (char)48;
        textTime[6]=(char)(sec%10) + (char)48;
    }
    else
    {
        textTime[5]=(char)48;
        textTime[6]=(char)(sec%10) + (char)48;
    }
    textTime[7]=',';
    textTime[8]=(char)((nSec/100000000)%10) + (char)48;
    textTime[9]=(char)((nSec/10000000)%10) + (char)48;
    textTime[10]='.';
    textTime[11]=(char)((nSec/1000000)%10) + (char)48;
    textTime[12]=(char)((nSec/100000)%10) + (char)48;
    textTime[13]='.';
    textTime[14]=(char)((nSec/10000)%10) + (char)48;
    textTime[15]=(char)((nSec/1000)%10) + (char)48;
    textTime[16]='.';
    textTime[17]=(char)((nSec/100)%10) + (char)48;
    textTime[18]=(char)((nSec/10)%10) + (char)48;
    textTime[19]=(char)(nSec%10) + (char)48;
    textTime[20]='\0';
}
//-----------------------------------------------------------------------
struct timespec timeDifference(struct timespec timeStart, struct timespec timeStop)
{
    struct timespec resultTimeDifference;
    if((timeStop.tv_nsec - timeStart.tv_nsec) < 0)
    {
        resultTimeDifference.tv_sec = timeStop.tv_sec - timeStart.tv_sec - 1;
        resultTimeDifference.tv_nsec = timeStop.tv_nsec - timeStart.tv_nsec + 1000000000;
    }
    else 
    {
        resultTimeDifference.tv_sec = timeStop.tv_sec - timeStart.tv_sec;
        resultTimeDifference.tv_nsec = timeStop.tv_nsec - timeStart.tv_nsec;
    }
    return resultTimeDifference;
}
//-------------------------------------------------------------------------------------------
int createFile()
{
	//Create path fo file
	int prefixLength = strlen(prefix);
	char* pathToFile = (char*)calloc(prefixLength+10,sizeof(char));
	sprintf(pathToFile,"../ODP/%s%03d",prefix,fileToWriteResultsNumber);
	fileToWriteResultsNumber++;

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	printf("%s path fo file\n",pathToFile);
	oldFileToWriteDescriptor = fileToWriteDescriptor;
	fileToWriteDescriptor = open(pathToFile, O_RDWR|O_CREAT|O_TRUNC, mode);
	if( fileToWriteDescriptor == -1 )
	{
		printf("Create file error: ");
		exit(EXIT_FAILURE);
	}
	dup2(fileToWriteDescriptor,oldFileToWriteDescriptor);
	printf("Created new file\n");

	//Check file read permission
	if(fcntl(fileToWriteDescriptor,F_GETFL) == -1)
	{
		perror("fcntl get flag createfile");
		exit(EXIT_FAILURE);
	}
	int permissions = fcntl(fileToWriteDescriptor,F_GETFL) & O_ACCMODE;

	//Search for file with read permission
	if( permissions != O_RDWR ) 
	{
		if(permissions != O_RDONLY)
		{
			free(pathToFile);
			close(fileToWriteDescriptor);
			//fileToWriteResultsNumber++;
			if(fileToWriteResultsNumber>999) { return -1; }
			if( createFile() == 0) 
				return 0;
			else 
				return -1; 
		}
	}

	if(fileToWriteResultsNumber>999) 
	{ 
		fileToWriteResultsNumber = 0; 
	}
	free(pathToFile);
	return 0;
}
//-----------------------------------------------------------------------------
void setSignalHandlerSIGUSR1CreateFile()
{
    struct sigaction sa;

    //handler for signal
   	sa.sa_flags = SA_RESTART;
    sa.sa_handler = &signalHandlerSIGUSR1CreateFile;

    if(sigaction(SIGUSR1,&sa,NULL) == -1) 
    { 
        perror("sigaction setSignalActionInfoFightEndSIGRTMIN");
        exit(EXIT_FAILURE); 
    }
}
//-----------------------------------------------------------------------
void signalHandlerSIGUSR1CreateFile(int sig)
{
	if( createFile() == -1)
	{
        printf("Can't create and find file with read permission.\nChange prefix\n");
        exit(EXIT_FAILURE);
    }
}