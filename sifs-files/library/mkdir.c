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
    //CHECKS EXISTING DIRECTORY NAME
    // FIXME  For now assume pathname is single string
    SIFS_DIRBLOCK dir_block = {
        .name = "a", //FIXME
        .modtime = time(NULL),
        .nentries = 0,
    };

    SIFS_VOLUME_HEADER header = getHeader(volumename);
    SIFS_BIT *bitmap = getBitmapPtr(volumename, header);

    //REVIEW Remove
    printf("%s\n", volumename);
    printf("%s\n", pathname);
    printf("%s\n", dir_block.name);
    printf("%ld\n", dir_block.modtime);
    printf("%d\n", dir_block.nentries);

    SIFS_errno = SIFS_ENOTYET;
    return 1;
}
