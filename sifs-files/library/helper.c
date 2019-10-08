/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"
#include <stdio.h>
#include <string.h>

// COMBINE FSEEK THE THEN READ
size_t fpread(FILE *fp, size_t offset, size_t start, void *buffer, size_t size, size_t nitems)
{
    if (fseek(fp, offset, start) != 0)
        return 0;
    return fread(buffer, size, nitems, fp);
}

SIFS_VOLUME_HEADER getHeader(const char *volumename)
{
    printf("%s\n", volumename);
    FILE *fp = fopen(volumename, "r+");
    if (fp == NULL)
    {
        //READING ERROR
        SIFS_errno = SIFS_ENOVOL;
        //return;
    }
    //READ HEADER
    SIFS_VOLUME_HEADER header;
    fread(&header, sizeof header, 1, fp);
    printf("blocksize=%i,  nblocks=%i\n", (int)header.blocksize, (int)header.nblocks);

    fclose(fp);
    return header;
}

SIFS_BIT *getBitmapPtr(const char *volumename, SIFS_VOLUME_HEADER header)
{
    printf("%s\n", volumename);
    FILE *fp = fopen(volumename, "r+");

    if (fp == NULL)
    {
        //READING ERROR
        SIFS_errno = SIFS_ENOVOL;
        return NULL;
    }
    SIFS_BIT *bitmap = malloc(header.nblocks * sizeof(int) + 1);

    //SKIP TO SPECIFIC BYTE COUNT THEN READ
    fpread(fp, sizeof(header), SEEK_SET, bitmap, header.nblocks, header.nblocks);

    printf("%s %d\n", bitmap, (int)(header.nblocks));

    //fclose(fp);
    return bitmap;
}

int getDirBlockIdByName(const char *volumename, int currentBlockID, const char *dirname)
{
    // REVIEW
    printf("CBID: %d, Dirname: %s\n", currentBlockID, dirname);
    return 0;
}

char file_arr[24][32];           //file, or rather variable that stores directory path name
char (*file_ptr)[32] = file_arr; //pointer to file array

PATH getSplitPath(const char *pathname)
{
    char *token;
    PATH path;
    path.numSubDir = 0;

    char *pathname_dup = malloc(strlen(pathname) + 1);
    strcpy(pathname_dup, pathname);

    //SPLIT THEN COPY TO STRING ARRAY
    token = strtok(pathname_dup, "/");
    while (token != NULL)
    {
        strcpy(path.subPathArray[path.numSubDir], token);
        path.numSubDir++;
        token = strtok(NULL, "/");
    }

    printf("PATH ARRAY: {\n");
    for (int i = 0; i < path.numSubDir; i++)
    {
        printf("%s\n", path.subPathArray[i]);
    }
    printf("\n");
    return path;
}
