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

    //PARSE PATH TO FIND AND READ ID
    SIFS_BLOCKID lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    char tailname[SIFS_MAX_NAME_LENGTH];
    strcpy(tailname, getPathTail(pathname));
    SIFS_BLOCKID tailId = getFileBlockIdByName(fp, lastPathHeadDirId, tailname);

    //ERROR CHECK OF THE PATH AND FILE EXISTENCE
    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }

    //INTERFACING POINTERS RETURNS
    SIFS_FILEBLOCK block = getFileBlockById(fp, tailId);
    *length = block.length;
    *modtime = block.modtime;
    return EXIT_SUCCESS;
}
