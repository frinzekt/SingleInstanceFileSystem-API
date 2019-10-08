/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
#include <stdio.h> //REVIEW  Remove
#include "sifs-internal.h"

// make a new directory within an existing volume
int SIFS_mkdir(const char *volumename, const char *pathname)
{
    //CHECKS EXISTING DIRECTORY NAME

    // FIXME  For now assume pathname is single string
    SIFS_DIRBLOCK dir_block = {
        .name = "a", //FIXME
        .modtime = time(NULL),
        .nentries = 0,
    };

    FILE *myvolume;
    myvolume = fopen(volumename, "w+");

    SIFS_VOLUME_HEADER header;
    fread(&header.blocksize, sizeof header.blocksize, 1, myvolume);
    fread(&header.nblocks, sizeof header.nblocks, 1, myvolume);

    //REVIEW Remove
    printf("%s\n", dir_block.name);
    printf("%ld\n", dir_block.modtime);
    printf("%d\n", dir_block.nentries);
    printf("%ld\n", header.blocksize);
    printf("%d\n", header.nblocks);

    SIFS_errno = SIFS_ENOTYET;
    return 1;
}
