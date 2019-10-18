/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"

// remove an existing directory from an existing volume
int SIFS_rmdir(const char *volumename, const char *pathname)
{
    FILE *fp = getFileReaderPointer(volumename);
    //PRE-PROCESSING ERROR CHECK - will return EXIT_FAILURE IN THE SCOPE OF THE API
CHECK_VOLUME_EXIST
CHECK_VALID_VOLUME

    char *tailname = getPathTail(pathname);
    SIFS_BLOCKID lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    SIFS_BLOCKID tailId = getDirBlockIdByName(fp, lastPathHeadDirId, tailname);

    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }

    removeDirBlock(fp, lastPathHeadDirId, tailId);

    fclose(fp);
    return EXIT_SUCCESS;
}
