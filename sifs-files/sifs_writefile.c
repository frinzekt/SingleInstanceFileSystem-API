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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

//  Compile with:  cc -std=c99 -Wall -Werror -pedantic -o square square.c
//  OR with alias mycc: mycc -o square square.c

int main(int argcount, char *argvalue[])
{
    // Check the number of command-line arguments
    if (argcount != 4)
    {
        fprintf(stderr, "Usage: %s [VolumeName] [PathName] [FileYouWantToCopy]\n", argvalue[0]);
        exit(EXIT_FAILURE); // Exit indicating failure
    }
    else
    {
        //ARGS: volumename pathname, file 1,2,3
        struct stat info;
        stat(argvalue[3], &info);
        FILE *fp = fopen(argvalue[3], "r+");
        void *data = malloc(info.st_size + 1);
        printf("INFOSIZE: %lld\n", info.st_size);
        fread(data, info.st_size, 1, fp);
        SIFS_writefile(argvalue[1], argvalue[2], data, info.st_size);

        fclose(fp);
        FILE *readfile = fopen("test.txt", "w");
        fwrite(data, 1, info.st_size, readfile);
        fclose(readfile);

        SIFS_perror(argvalue[0]);
    }
    return 0;
}
