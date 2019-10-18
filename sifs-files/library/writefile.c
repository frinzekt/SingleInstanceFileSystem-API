/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"

// add a copy of a new file to an existing volume
int SIFS_writefile(const char *volumename, const char *pathname,
                   void *data, size_t nbytes)
{
    FILE *fp = getFileReaderPointer(volumename);
    //PRE-PROCESSING ERROR CHECK - will return EXIT_FAILURE IN THE SCOPE OF THE API
    CHECK_VOLUME_EXIST
    CHECK_VALID_VOLUME

    SIFS_BLOCKID containerId = getDirBlockIdBeforePathEnds(fp, pathname);
    char *tail = getPathTail(pathname);

    if (writeFileBlock(fp, containerId, tail, data, nbytes))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
