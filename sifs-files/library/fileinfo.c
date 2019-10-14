/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include <stdio.h>
#include <string.h>
#include "sifs-internal.h"
#include "helper.h"

// get information about a requested file
int SIFS_fileinfo(const char *volumename, const char *pathname,
                  size_t *length, time_t *modtime)
{
    FILE *fp = getFileReaderPointer(volumename);
    CHECK_VOLUME_EXIST

    SIFS_BLOCKID lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    char tailname[SIFS_MAX_NAME_LENGTH];
    strcpy(tailname, getPathTail(pathname));
    printf("head: %i, tail:%s \n", lastPathHeadDirId, tailname);

    SIFS_BLOCKID tailId = getFileBlockIdByName(fp, lastPathHeadDirId, tailname);
    printf("head: %i, tail: %i-%s \n", lastPathHeadDirId, tailId, tailname);
    printf("SIFS ERROR: %i \n", SIFS_errno);

    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }

    SIFS_FILEBLOCK block = getFileBlockById(fp, tailId);
    *length = block.length;
    *modtime = block.modtime;
    printf("START HERE %li, %li\n", block.length, block.modtime);

    printf("API OUTPUT STARTS HERE:\n");
    printf("length: %ld ", *length);
    printf("modtime: %ld", *modtime);
    printf("API OUTPUT ENDS HERE:\n");

    return EXIT_SUCCESS;
}
