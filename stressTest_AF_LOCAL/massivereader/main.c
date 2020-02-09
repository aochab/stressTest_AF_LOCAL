#include "massivereader.h"

int main(int argc, char* argv[])
{
    if(argc<3)
    {
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }

    getParameters(argc,argv);

    printf("Port %d prefix %s",portNr,prefix);
    free(prefix);
    exit(EXIT_SUCCESS);
}