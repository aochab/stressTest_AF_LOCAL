#include "multiwriter.h"

int main(int argc, char* argv[])
{
    createClientINET();
    communicationINET(client_fd);


    printf("multiwriter");
    close(client_fd);
    exit(EXIT_SUCCESS);
}