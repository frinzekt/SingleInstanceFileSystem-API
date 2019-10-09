/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"
#include <stdio.h>
#include <string.h>

FILE *getFileReaderPointer(const char *volumename)
{
    FILE *fp = fopen(volumename, "r+");
    if (fp == NULL)
    {
        //READING ERROR
        SIFS_errno = SIFS_ENOVOL;
        return NULL;
    }
    return fp;
}
void resetFilePointerToStart(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
}
// COMBINE FSEEK THE THEN READ
size_t fpread(FILE *fp, size_t offset, size_t start, void *buffer, size_t size, size_t nitems)
{
    if (fseek(fp, offset, start) != 0)
    {
        printf("ERROR");
        return 0;
    }
    return fread(buffer, size, nitems, fp);
}

SIFS_VOLUME_HEADER getHeader(FILE *fp)
{
    //READ HEADER
    SIFS_VOLUME_HEADER header;
    fread(&header, sizeof header, 1, fp);
    printf("blocksize=%i,  nblocks=%i\n", (int)header.blocksize, (int)header.nblocks);

    resetFilePointerToStart(fp);
    return header;
}

SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header)
{
    SIFS_BIT *bitmap = malloc(header.nblocks * sizeof(int) + 1);

    //SKIP TO SPECIFIC BYTE COUNT THEN READ
    //fpread(fp, sizeof(header), SEEK_SET, bitmap, header.nblocks, header.nblocks);
    fseek(fp, sizeof(header), SEEK_SET);
    fread(bitmap, 1, header.nblocks, fp);
    printf("%s %d\n", bitmap, (int)(header.nblocks));

    resetFilePointerToStart(fp);
    return bitmap;
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
SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockID)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header); //REVIEW , bitmap never used
    SIFS_DIRBLOCK *blockptr = malloc(header.blocksize + 1);

    //OFFSET... header size, bitmap size, rootdir size, sizes of previous block
    //roodir = sizes of previous block => (1+blockID-1)
    int offset = sizeof(header) + header.nblocks + (currentBlockID) * (header.blocksize);
    printf("OFFSET: %i bitmap:%c\n", offset, bitmap[currentBlockID]);

    //fpread(fp, offset, SEEK_SET, &block, sizeof(block), 1);
    fseek(fp, offset, SEEK_SET);
    fread(blockptr, header.blocksize, 1, fp);

    printf("name: %s,modtime:%ld, nentries:%d, size of data: %ld  \n\n", blockptr->name, blockptr->modtime, blockptr->nentries, sizeof(SIFS_DIRBLOCK));

    resetFilePointerToStart(fp);
    return *blockptr;
}
int getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname)
{
    // REVIEW
    printf("CBID: %d, Dirname: %s\n", currentBlockID, dirname);
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    //OFFSET... header size, bitmap size, rootdir size, sizes of previous block
    //roodir = sizes of previous block => (1+blockID-1)
    int offset = sizeof(header) + header.nblocks + (currentBlockID) * (header.blocksize);
    printf("OFFSET: %i bitmap:%c\n", offset, bitmap[1]);

    return 0;
}
