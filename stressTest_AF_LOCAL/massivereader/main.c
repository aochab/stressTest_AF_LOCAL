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
    communicationINET();

 ///   printf("Port %d prefix %s",portNr,prefix);
    close(server_fd);
    free(prefix);
    exit(EXIT_SUCCESS);
}