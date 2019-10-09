/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include <stdio.h>
#include <time.h>

#include "sifs-internal.h"
#include "helper.h"

// void info_print(void)
// {
//     //printf("contents of blockID = %i:\n\n", /*number*/);
//     printf("struct {\n");
//     //printf("\t\tname = \"%c\"\n", /*dirname*/);
//     //printf("modified = %i (%c)\n", /*unix*/ /*actual date n time*/);
//     // printf("nentries = %i\n\n", /*entry no*/);

//     for (int i = 0; i < SIFS_MAX_ENTRIES; i++)
//     {
//         //printf("\tentries[%2i] = %c\n", /*block file index*/)
//     }

//     printf("} SIFS_DIRBLOCK;\n");
// }

// get information about a requested directory
int SIFS_dirinfo(const char *volumename, const char *pathname,
                 char ***entrynames, uint32_t *nentries, time_t *modtime)
{
    FILE *fp = getFileReaderPointer(volumename);
    //NULL CHECKER ... return 1 - failure

    SIFS_BLOCKID lastPathHeadDirId = getDirBlockIdBeforePathEnds(fp, pathname);
    char *tailname = getPathTail(pathname);
    const char *newtail = tailname;
    printf("head: %i, tail:%s \n", lastPathHeadDirId, tailname);
    printf("head: %i, tail:%s \n", lastPathHeadDirId, newtail);
    SIFS_BLOCKID tailId = getDirBlockIdByName(fp, lastPathHeadDirId, pathname); //FIXME
    printf("head: %i, tail: %i-%s \n", lastPathHeadDirId, tailId, tailname);
    printf("SIFS ERROR: %i \n", SIFS_errno);

    if ((tailId == -1) || (lastPathHeadDirId == -1))
    {
        return 1;
    }

    SIFS_DIRBLOCK block = getDirBlockById(fp, tailId);
    *nentries = block.nentries;
    *modtime = block.modtime; //FIXME

    printf("SUCCESS DIRINFO\n");
    printf("blockname: %s \n", block.name);
    printf("nentries: %d = %d\n", *nentries, block.nentries);
    printf("modtime : %ld = %ld\n", *modtime, block.modtime);
    printf("LIBRARY OUTPUT ENDS HERE -------");

    //TRIPLE POINTER
    //EntryIDs -> BlockID -> fileindex -> storing value
    // * -> Strings
    //
    fclose(fp);
    return 0;
}
/*
make remake

running exe
./sifs_mkvolume [volume name] [block size] [block no]
./sifs_mkvolume  2000 10
./sifs_dirinfo sample/volD subdir1
*/