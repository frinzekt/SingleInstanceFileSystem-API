/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"
#include <string.h>

// read the contents of an existing file from an existing volume
int SIFS_readfile(const char *volumename, const char *pathname,
                  void **data, size_t *nbytes)
{
    FILE *fp = getFileReaderPointer(volumename);
    //PRE-PROCESSING ERROR CHECK - will return EXIT_FAILURE IN THE SCOPE OF THE API
CHECK_VOLUME_EXIST
CHECK_VALID_VOLUME
    SIFS_BLOCKID tailId;
    SIFS_BLOCKID lastPathHeadDirId;
    char readerfile[SIFS_MAX_NAME_LENGTH];

    lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    char tailname[SIFS_MAX_NAME_LENGTH];
    strcpy(tailname, getPathTail(pathname));
    tailId = getFileBlockIdByName(fp, lastPathHeadDirId, tailname);
    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }
    strcpy(readerfile, pathname);

    SIFS_FILEBLOCK target = getFileBlockById(fp, tailId);
    *nbytes = target.length;
    *data = malloc(*nbytes + 1);
    fseek(fp, getOffset(fp, target.firstblockID), SEEK_SET);
    fread(*data, 1, *nbytes, fp);

    fclose(fp);
    return EXIT_SUCCESS;
}
