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
		printf("From serwer INET : %d\n",response.sun_family);
	}
	
}
//-------------------------------------------------------------------------------
void createSerwerLOCAL(struct sockaddr_un *mainServerLOCALAddress, int *mainServerLocal_fd)
{
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
//-----------------------------------------------------------------------------------
void changeUnitsMicrosecToSecAndNsec(float inputMicroSec, long* outSec, long *outNsec)
{
	float integerPart;
	float decimalPart = modff(inputMicroSec,&integerPart);
    if(integerPart<1000000)
    {
        *outNsec = integerPart * 1000;
    } 
    else
    {//Unit conversion microsec on sec and nsec
        if(fmod(integerPart,1000000)!=0) 
        { 
            *outNsec = fmod(integerPart,1000000);
            integerPart -= *outNsec;
            *outNsec *= 1000;
        }
        int multiplier = 0;
        while(fmod(integerPart,1000000)==0)
        {
            integerPart /= 1000000;
            multiplier++;
        }
        multiplier--;
        for(int i=0;i<multiplier;i++)
        {
            integerPart *= 1000000;
        }
        *outSec = integerPart;
		*outNsec = decimalPart*1000;
    } 
}
//-----------------------------------------------------------------------------------
void changeUnitsCentisecToSecAndNsec(float inputCentiSec, long *outSec, long *outNsec)
{
	float integerPart;
	float decimalPart = modff(inputCentiSec,&integerPart);
    if(integerPart<100)
    {
        *outNsec = integerPart * 10000000;
    } 
    else
    {//Unit conversion ds on sec and nsec
        if(fmod(integerPart,100)!=0) 
        { 
            *outNsec = fmod(integerPart,10);
            integerPart -= *outNsec;
            *outNsec *= 10000000;
        }
        int multiplier = 0;
        while(fmod(integerPart,100)==0)
        {
            integerPart /= 100;
            multiplier++;
        }
        multiplier--;
        for(int i=0;i<multiplier;i++)
        {
            integerPart *= 100;
        }
        *outSec = integerPart;
		*outNsec = decimalPart*10000000;
    } 
}
//--------------------------------------------------------------------------------------
void sendMessage(struct sockaddr_un mainServerLOCALAddress)
{
        nanosleep(&timeIntervalBeetwenMsgConverted,0);
        
        Message msg;

        //Take time and convert it to textual
        struct timespec sendMsgTime;
        if(clock_gettime(CLOCK_REALTIME,&sendMsgTime) == -1) 
        {
            perror("clock_getime sendMessage");
            exit(EXIT_FAILURE);
        }
        //Time from program start to sending the message
        struct timespec wallTimeSendMsg = timeDifference(startProgramTime,sendMsgTime);
        char* textTime = (char*)calloc(TEXT_TIME_REPRESENATION,sizeof(char));
        makeTextualRepresentationOfTime(textTime,wallTimeSendMsg);
        printf("%s\n",textTime);
        strncpy(msg.textTime,textTime,TEXT_TIME_REPRESENATION+1);

        //Random socket
        int i = rand()%acceptedConnectionsLOCAL; 

        //Send structure wall clock
        msg.time = sendMsgTime;
        memset(&msg.socketPath,0,sizeof(msg.socketPath));
        strncpy(msg.socketPath,mainServerLOCALAddress.sun_path,sizeof(mainServerLOCALAddress.sun_path));

        write(localsFds[i],&msg,sizeof(Message));

        struct timespec stopSendMsgTime;
        if(clock_gettime(CLOCK_REALTIME,&stopSendMsgTime) == -1) 
        {
            perror("clock_getime sendMessage");
            exit(EXIT_FAILURE);
        }

        struct timespec msgSendTime = timeDifference(sendMsgTime,stopSendMsgTime);
        makeInfoAboutSendingMessagesTime(msgSendTime);
        
        free(textTime);
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
        minutes = timeStruct.tv_sec/60;
    }
	if(minutes>9999999)
	{
	    textTime[0]=(char)((minutes/10000000)%10) + (char)48;
	    textTime[1]=(char)((minutes/1000000)%10) + (char)48;
        textTime[2]=(char)((minutes/100000)%10) + (char)48;
		textTime[3]=(char)((minutes/10000)%10) + (char)48;
		textTime[4]=(char)((minutes/1000)%10) + (char)48;
        textTime[5]=(char)((minutes/100)%10) + (char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
	} 
	else if(minutes>999999)
	{
        textTime[0]=(char)48;
	    textTime[1]=(char)((minutes/1000000)%10) + (char)48;
        textTime[2]=(char)((minutes/100000)%10) + (char)48;
		textTime[3]=(char)((minutes/10000)%10) + (char)48;
		textTime[4]=(char)((minutes/1000)%10) + (char)48;
        textTime[5]=(char)((minutes/100)%10) + (char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
	}
	else if(minutes>99999)
	{
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)((minutes/100000)%10) + (char)48;
		textTime[3]=(char)((minutes/10000)%10) + (char)48;
		textTime[4]=(char)((minutes/1000)%10) + (char)48;
        textTime[5]=(char)((minutes/100)%10) + (char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
	}
	else if(minutes>9999)
	{
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)48;
		textTime[3]=(char)((minutes/10000)%10) + (char)48;
		textTime[4]=(char)((minutes/1000)%10) + (char)48;
        textTime[5]=(char)((minutes/100)%10) + (char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
	}
	else if(minutes>999)
	{
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)48;
        textTime[3]=(char)48;
		textTime[4]=(char)((minutes/1000)%10) + (char)48;
        textTime[5]=(char)((minutes/100)%10) + (char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
	}
    else if(minutes>99)
    {
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)48;
        textTime[3]=(char)48;
        textTime[4]=(char)48;
        textTime[5]=(char)((minutes/100)%10) + (char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
    }
    else if(minutes>9)
    {
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)48;
        textTime[3]=(char)48;
        textTime[4]=(char)48;
        textTime[5]=(char)48;
        textTime[6]=(char)((minutes/10)%10) + (char)48;
        textTime[7]=(char)(minutes%10) + (char)48;
    }
    else
    {
        textTime[0]=(char)48;
        textTime[1]=(char)48;
        textTime[2]=(char)48;
        textTime[3]=(char)48;
        textTime[4]=(char)48;
        textTime[5]=(char)48;
        textTime[6]=(char)48;
        textTime[7]=(char)(minutes%10)+ (char)48;
    }
    textTime[8]='*'; textTime[9]=':';
    if(sec>9)
    {
        textTime[10]=(char)(sec/10) + (char)48;
        textTime[11]=(char)(sec%10) + (char)48;
    }
    else
    {
        textTime[10]=(char)48;
        textTime[11]=(char)(sec%10) + (char)48;
    }
    textTime[12]=',';
    textTime[13]=(char)((nSec/100000000)%10) + (char)48;
    textTime[14]=(char)((nSec/10000000)%10) + (char)48;
    textTime[15]='.';
    textTime[16]=(char)((nSec/1000000)%10) + (char)48;
    textTime[17]=(char)((nSec/100000)%10) + (char)48;
    textTime[18]='.';
    textTime[19]=(char)((nSec/10000)%10) + (char)48;
    textTime[20]=(char)((nSec/1000)%10) + (char)48;
    textTime[21]='.';
    textTime[22]=(char)((nSec/100)%10) + (char)48;
    textTime[23]=(char)((nSec/10)%10) + (char)48;
    textTime[24]=(char)(nSec%10) + (char)48;
    textTime[25]='\0';
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
//---------------------------------------------------------------------------
void makeInfoAboutSendingMessagesTime(struct timespec sendMsgTime)
{
        //Sum total send messagges time
        totalSendMsgTime.tv_nsec += sendMsgTime.tv_nsec;
        if(totalSendMsgTime.tv_nsec > 1000000000 )
        {
            totalSendMsgTime.tv_sec += sendMsgTime.tv_sec;
            totalSendMsgTime.tv_sec += 1;
            totalSendMsgTime.tv_nsec -= 1000000000;
        }
        else
        {
            totalSendMsgTime.tv_sec += sendMsgTime.tv_sec;
        }

        //Find min and max send time
        if( sendMsgTime.tv_sec > maxSendMsgTime.tv_sec )
        {
            maxSendMsgTime = sendMsgTime;

        } else if( sendMsgTime.tv_sec == maxSendMsgTime.tv_sec )
        {
            if( sendMsgTime.tv_nsec > maxSendMsgTime.tv_nsec )
            {
                maxSendMsgTime = sendMsgTime;
            }
        } 
        
        if( sendMsgTime.tv_sec < minSendMsgTime.tv_sec )
        {
            minSendMsgTime = sendMsgTime;

        } else if( sendMsgTime.tv_sec == minSendMsgTime.tv_sec )
        {
            if( sendMsgTime.tv_nsec < minSendMsgTime.tv_nsec )
            {
                minSendMsgTime = sendMsgTime;
            }
        } 
}
//--------------------------------------------------------------------------
void setTimer()
{
    struct sigevent sev;
    struct sigaction sa;
    struct itimerspec its;
    timer_t timerid;

    //handler for signal
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = signalHandler;

    if(sigaction(SIGTERM,&sa,NULL) == -1) 
    { 
        perror("sigaction setTimer");
        exit(EXIT_FAILURE); 
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGTERM;
    sev.sigev_value.sival_ptr = &timerid;

    if(timer_create(CLOCK_REALTIME,&sev,&timerid) == -1) 
    { 
        perror("timer create setTimer"); 
        exit(EXIT_FAILURE);
    }

    its.it_value.tv_sec = timeTotalWorkConvertConverted.tv_sec;
    its.it_value.tv_nsec = timeTotalWorkConvertConverted.tv_nsec;
    its.it_interval.tv_sec = timeTotalWorkConvertConverted.tv_sec; 
    its.it_interval.tv_nsec = timeTotalWorkConvertConverted.tv_nsec;

    if(timer_settime(timerid,0,&its,NULL) == -1) 
    { 
        perror("settime setTimer"); 
        exit(EXIT_FAILURE);
    }
}
//-----------------------------------------------------------------------
void signalHandler(int sig)
{
	if(clock_gettime(CLOCK_REALTIME,&stopSendMessagesTime) == -1) 
    {
        perror("clock_getime stopSendMessagesTime");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
//---------------------------------------------------
void exitFunction(void)
{
    struct timespec totalProgramTimeWoks = timeDifference(startSendMessagesTime,stopSendMessagesTime);
    char* textTime = (char*)calloc(TEXT_TIME_REPRESENATION,sizeof(char));;
    makeTextualRepresentationOfTime(textTime,totalProgramTimeWoks);
    printf("Total time for sending messages %s\n",textTime);

    minSendMsgTime.tv_nsec += timeIntervalBeetwenMsgConverted.tv_nsec;
    if(minSendMsgTime.tv_nsec > 1000000000 )
    {
        minSendMsgTime.tv_sec += timeIntervalBeetwenMsgConverted.tv_sec;
        minSendMsgTime.tv_sec += 1;
        minSendMsgTime.tv_nsec -= 1000000000;
    }
    else
    {
        minSendMsgTime.tv_sec += timeIntervalBeetwenMsgConverted.tv_sec;
    }
    char* textTimeMIN = (char*)calloc(TEXT_TIME_REPRESENATION,sizeof(char));;
    makeTextualRepresentationOfTime(textTimeMIN,minSendMsgTime);
    printf("Minimal time interval between cycles %s\n",textTimeMIN);


    maxSendMsgTime.tv_nsec += timeIntervalBeetwenMsgConverted.tv_nsec;
    if(maxSendMsgTime.tv_nsec > 1000000000 )
    {
        maxSendMsgTime.tv_sec += timeIntervalBeetwenMsgConverted.tv_sec;
        maxSendMsgTime.tv_sec += 1;
        maxSendMsgTime.tv_nsec -= 1000000000;
    }
    else
    {
        maxSendMsgTime.tv_sec += timeIntervalBeetwenMsgConverted.tv_sec;
    }
    char* textTimeMAX = (char*)calloc(TEXT_TIME_REPRESENATION,sizeof(char));;
    makeTextualRepresentationOfTime(textTimeMAX,maxSendMsgTime);
    printf("Maximum time interval between cycles %s\n",textTimeMAX);

    free(textTime);
    free(textTimeMIN);
    free(textTimeMAX);
}