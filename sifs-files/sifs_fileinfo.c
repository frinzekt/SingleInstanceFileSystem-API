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
    if (argcount != 3)
    {
        fprintf(stderr, "Usage: %s [VolumeName] [PathName]\n", argvalue[0]);
        exit(EXIT_FAILURE); // Exit indicating failure
    }
    else
    {
        //CHANGE WHATEVER IS IN HERE
        size_t *length = malloc(10000);
        time_t *modtime = malloc(10000);
        //CHANGE WHATEVER IS IN HERE
        SIFS_fileinfo(argvalue[1], argvalue[2],
                      length, modtime);

        printf("IN SIFS_FILEINFO %ld, %ld\n", *length, *modtime);
        SIFS_perror(argvalue[0]);
    }
    return 0;
}