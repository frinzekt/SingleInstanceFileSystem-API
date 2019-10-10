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
    printf("\tmodified = %ld (%s)\n", block->modtime, ctime(&(block->modtime)));
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
    //NULL CHECKER ... return 1 - failure

    SIFS_BLOCKID lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    char tailname[SIFS_MAX_NAME_LENGTH];
    strcpy(tailname, getPathTail(pathname));
    printf("head: %i, tail:%s \n", lastPathHeadDirId, tailname);
    SIFS_BLOCKID tailId = getDirBlockIdByName(fp, lastPathHeadDirId, tailname);
    printf("head: %i, tail: %i-%s \n", lastPathHeadDirId, tailId, tailname);
    printf("SIFS ERROR: %i \n", SIFS_errno);

    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return 1;
    }

    SIFS_DIRBLOCK block = getDirBlockById(fp, tailname);
    *nentries = block.nentries;
    *modtime = block.modtime;

    printf("SUCCESS DIRINFO\n");
    info_print(&block);
    //     printf("blockname: %s \n", block.name);
    // printf("nentries: %d = %d\n", *nentries, block.nentries);
    //printf("modtime : %ld = %ld\n", *modtime, block.modtime);

    //TRIPLE POINTER
    //EntryIDs -> BlockID -> fileindex -> storing value
    //??->StringArray->String
    // * -> Strings
    //
    char **found = malloc((block.nentries + 1) * sizeof(SIFS_MAX_NAME_LENGTH));
    for (int i = 0; i < block.nentries; i++)
    {
        //NOTE Strdup is not in ISO C standard (its a POSIX thing)
        char *name = getBlockNameById(fp, block.entries[i].blockID, block.entries[i].fileindex);
        found[i] = malloc(strlen(name) + 1);
        if (found[i] == NULL)
            return -1; // No memory

        strcpy(found[i], name);
        printf("entry no %i: %s\n", i, found[i]);
    }
    *entrynames = found;
    printf("LIBRARY OUTPUT ENDS HERE -------");
    fclose(fp);
    return 1;
}
/*
make remake

running exe
./sifs_mkvolume [volume name] [block size] [block no]
./sifs_mkvolume  2000 10
./sifs_dirinfo sample/volD subdir1
*/