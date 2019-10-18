/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "sifs-internal.h"
#include "helper.h"
#if defined(__linux__)
extern char *strdup(const char *s);
#endif

// get information about a requested directory
int SIFS_dirinfo(const char *volumename, const char *pathname,
                 char ***entrynames, uint32_t *nentries, time_t *modtime)
{
    FILE *fp = getFileReaderPointer(volumename);
    //PRE-PROCESSING ERROR CHECK - will return EXIT_FAILURE IN THE SCOPE OF THE API
    CHECK_VOLUME_EXIST
    CHECK_VALID_VOLUME

    SIFS_BLOCKID tailId;
    SIFS_BLOCKID lastPathHeadDirId = 0;

    //CHECKS CONDITION FOR PATHNAME ARGUMENT, 0 RETURNS ROOT DIRECTORY
    if (strcmp(pathname, "0") != 0) //ROOT SELECTION
    {                               //DOES EXECUTES IN HERE, BUT PRODUCES ERROR WHEN PASSING TO SIFS_DIRINFO
        lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
        char *tailname = getPathTail(pathname);
        tailId = getDirBlockIdByName(fp, lastPathHeadDirId, tailname);
    }
    else
    {
        tailId = SIFS_ROOTDIR_BLOCKID;
    }

    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }

    SIFS_DIRBLOCK block = getDirBlockById(fp, tailId);
    *nentries = block.nentries;
    *modtime = block.modtime;

    //ALLOCATION OF NAMES INTO VARIABLES
    char **found = malloc((block.nentries + 1) * sizeof(SIFS_MAX_NAME_LENGTH));
    for (int i = 0; i < block.nentries; i++)
    {
        char *name = getBlockNameById(fp, block.entries[i].blockID, block.entries[i].fileindex);
        found[i] = strdup(name);
    }

    *entrynames = found;
    fclose(fp);
    return EXIT_SUCCESS;
}
