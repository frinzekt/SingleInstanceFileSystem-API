/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
#include <stdio.h> //REVIEW
#include "sifs-internal.h"
#include "helper.h"

// make a new directory within an existing volume
int SIFS_mkdir(const char *volumename, const char *pathname)
{
    FILE *fp = getFileReaderPointer(volumename);
    SIFS_BLOCKID container = getDirBlockIdBeforePathEnds(fp, pathname);
    char *tail = getPathTail(pathname);
    printf("TAIL HERE %s\n", tail);
    writeDirBlock(fp, container, tail);
    fclose(fp);
    return 1;
}
