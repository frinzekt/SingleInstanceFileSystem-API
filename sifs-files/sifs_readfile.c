//title:      playground.c
//Written By: Frinze Lapuz
//date:       2019-10-08 09:17:59
//Generated using custom user Snippet

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "sifs.h"

//  Compile with:  cc -std=c99 -Wall -Werror -pedantic -o square square.c
//  OR with alias mycc: mycc -o square square.c

int main(int argcount, char *argvalue[])
{
    // Check the number of command-line arguments
    if (argcount != 4)
    {
        fprintf(stderr, "Usage: %s [VolumeName] [PathName] [FileYouWantToReadFrom]\n", argvalue[0]);
        exit(EXIT_FAILURE); // Exit indicating failure
    }
    else
    {
        void *data;
        size_t nbytes;
        SIFS_readfile(argvalue[1], argvalue[2], &data, &nbytes);

        FILE *readfile = fopen(argvalue[3], "w");
        fwrite(data, 1, nbytes, readfile);
        fclose(readfile);
        printf("Success\n");
    }
    exit(EXIT_SUCCESS);
    return 0;
}
