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

void info_print(SIFS_DIRBLOCK *block)
{
    //printf("contents of blockID = %i:\n\n", /*number*/);
    printf("struct {\n");
    printf("\t    name = \"%s\"\n", block->name);
    printf("\tmodified = %ld - %s", block->modtime, ctime(&(block->modtime)));
    printf("\tnentries = %i\n\n", block->nentries);

    // for (int i = 0; i < SIFS_MAX_ENTRIES; i++)
    // {
    //     printf("\tentries[%2i] = %c\n", /*block file index*/)
    // }

    printf("} SIFS_DIRBLOCK;\n");
}

// get information about a requested directory
int SIFS_dirinfo(const char *volumename, const char *pathname,
                 char ***entrynames, uint32_t *nentries, time_t *modtime)
{
    FILE *fp = getFileReaderPointer(volumename);
    CHECK_VOLUME_EXIST

    SIFS_BLOCKID tailId;
    SIFS_BLOCKID lastPathHeadDirId = 0;
    if (strcmp(pathname, "0") != 0) //ROOT SELECTION
    {                               //DOES EXECUTES IN HERE, BUT PRODUCES ERROR WHEN PASSING TO SIFS_DIRINFO
        lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
        char *tailname = getPathTail(pathname);
        tailId = getDirBlockIdByName(fp, lastPathHeadDirId, tailname);

        printf("SIFS ERROR: %i \n", SIFS_errno);
    }
    else
    {
        tailId = SIFS_ROOTDIR_BLOCKID;
    }

    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return EXIT_FAILURE;
    }

    //INTERFACING POINTERS
    SIFS_DIRBLOCK block = getDirBlockById(fp, tailId);
    *nentries = block.nentries;
    *modtime = block.modtime;

    printf("SUCCESS DIRINFO\n");
    info_print(&block);

    //TRIPLE POINTER
    //*->StringArray->String
    char **found = malloc((block.nentries + 1) * sizeof(SIFS_MAX_NAME_LENGTH));
    for (int i = 0; i < block.nentries; i++)
    {
        //NOTE Strdup is not in ISO C standard (its POSIX)
        char *name = getBlockNameById(fp, block.entries[i].blockID, block.entries[i].fileindex);
        found[i] = malloc(strlen(name) + 1);
        if (found[i] == NULL)
            return EXIT_FAILURE; // No memory

        strcpy(found[i], name);
        printf("entry no %i: %20s\tblockID = %i\n", i, found[i], block.entries[i].blockID);
    }
    *entrynames = found;
    printf("LIBRARY OUTPUT ENDS HERE -------\n");
    fclose(fp);
    return EXIT_SUCCESS;
}
