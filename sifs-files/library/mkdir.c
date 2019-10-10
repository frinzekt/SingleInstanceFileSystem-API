/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
#include <stdio.h> //REVIEW
#include "sifs-internal.h"
#include "helper.h"

// make a new directory within an existing volume
int SIFS_mkdir(const char *volumename, const char *pathname)
{
    FILE *fp = getFileReaderPointer(volumename);
    printf("RESULT: %i\n", getDirBlockIdBeforePathEnds(fp, pathname));
    // FIXME  For now assume pathname is single string
    SIFS_DIRBLOCK dir_block = {
        .name = "a", //FIXME
        .modtime = time(NULL),
        .nentries = 0,
    };

    SIFS_VOLUME_HEADER header = getHeader(fp);
    printf("mkdir: ");
    printf("blocksize=%i,  nblocks=%i\n", (int)header.blocksize, (int)header.nblocks);

    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    printf("mkdir: ");
    printf("%s %d\n", bitmap, (int)(header.nblocks));

    //REVIEW Remove
    printf("%s\n", volumename);
    printf("%s\n", pathname);
    printf("%s\n", dir_block.name);
    printf("%ld\n", dir_block.modtime);
    printf("%d\n", dir_block.nentries);

    SIFS_errno = SIFS_ENOTYET;
    fclose(fp);
    return 1;
}
