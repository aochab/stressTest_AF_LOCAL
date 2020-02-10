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
    acceptResponseINET();

    //new LOCAL Socket
    struct sockaddr_un clientLOCALAdress;
	int clientLocal_fd;

    communicationINET(&clientLOCALAdress,&clientLocal_fd);
    communicationLOCAL(clientLOCALAdress,clientLocal_fd);

 ///   printf("Port %d prefix %s",portNr,prefix);
    close(server_fd);
    free(prefix);
    exit(EXIT_SUCCESS);
}