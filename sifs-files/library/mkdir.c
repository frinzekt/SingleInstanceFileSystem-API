/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
#include <stdio.h>
#include "sifs-internal.h"
#include "helper.h"

// make a new directory within an existing volume
int SIFS_mkdir(const char *volumename, const char *pathname)
{
    FILE *fp = getFileReaderPointer(volumename);
    //PRE-PROCESSING ERROR CHECK - will return EXIT_FAILURE IN THE SCOPE OF THE API
    CHECK_VOLUME_EXIST
    CHECK_VALID_VOLUME

    SIFS_BLOCKID containerId = getDirBlockIdBeforePathEnds(fp, pathname);
    char *tail = getPathTail(pathname);

    //PATH ERROR CHECK
    if (containerId == -1)
    {
        return EXIT_FAILURE;
    }

    if (!writeDirBlock(fp, containerId, tail))
    { //ERROR CHECKS
        return EXIT_FAILURE;
    }

    fclose(fp);
    return EXIT_SUCCESS; //SUCCESS
}
