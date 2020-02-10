#include "multiwriter.h"

int main(int argc, char* argv[])
{
    createClientINET();
    communicationINET();

    printf("multiwriter");
    close(client_fd);
    exit(EXIT_SUCCESS);
}