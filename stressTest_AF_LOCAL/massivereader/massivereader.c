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