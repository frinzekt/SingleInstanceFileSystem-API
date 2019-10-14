/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"
#include <string.h>

// remove an existing file from an existing volume
int SIFS_rmfile(const char *volumename, const char *pathname)
{
    FILE *fp = getFileReaderPointer(volumename);
    CHECK_VOLUME_EXIST

    //PARSE PATH TO FIND AND READ ID
    SIFS_BLOCKID lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    char tailname[SIFS_MAX_NAME_LENGTH];
    strcpy(tailname, getPathTail(pathname));
    SIFS_BLOCKID tailId = getFileBlockIdByName(fp, lastPathHeadDirId, tailname);
    uint32_t tailIndex = getFileBlockIndexByName(fp, lastPathHeadDirId, tailname);

    //ERROR CHECK OF THE PATH AND FILE EXISTENCE
    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }
    if (removeFileBlockById(fp, lastPathHeadDirId, tailId, tailIndex))
    {
        return EXIT_FAILURE;
    }

    SIFS_errno = SIFS_ENOTYET;
    return 1;
}
