#include "multiwriter.h"

int main(int argc, char* argv[])
{
    createClientINET();

    //----------------------
    //new socket LOCAL
    int serverLocal_fd;
	int clientLocal_fd;

    communicationINET(&serverLocal_fd,&clientLocal_fd);
    communicationLOCAL(clientLocal_fd);

    printf("multiwriter");
    close(client_fd);
 
    exit(EXIT_SUCCESS);
}