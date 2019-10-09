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
    //printf("blocksize=%i,  nblocks=%i\n", (int)header.blocksize, (int)header.nblocks);

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
    //printf("%s %d\n", bitmap, (int)(header.nblocks));

    resetFilePointerToStart(fp);
    return bitmap;
}

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

#define READ_OFFSET sizeof(header) + header.nblocks + (currentBlockID) * (header.blocksize)

SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockID)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    //SIFS_BIT *bitmap = getBitmapPtr(fp, header); //REVIEW , Assume IS DIRBLOCK
    SIFS_DIRBLOCK *blockptr = malloc(header.blocksize + 1);

    //OFFSET... header size, bitmap size, rootdir size, sizes of previous block
    //roodir = sizes of previous block => (1+blockID-1)
    int offset = READ_OFFSET;
    //printf("OFFSET: %i bitmap:%c\n", offset, bitmap[currentBlockID]);

    //File Read
    fseek(fp, offset, SEEK_SET);
    fread(blockptr, header.blocksize, 1, fp);

    resetFilePointerToStart(fp);
    return *blockptr;
}
SIFS_FILEBLOCK getFileBlockById(FILE *fp, SIFS_BLOCKID currentBlockID)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header); //REVIEW , Assume IS FILEBLOCK
    SIFS_FILEBLOCK *blockptr = malloc(header.blocksize + 1);

    //OFFSET... header size, bitmap size, rootdir size, sizes of previous block
    //roodir = sizes of previous block => (1+blockID-1)
    int offset = READ_OFFSET;
    printf("OFFSET: %i bitmap:%c\n", offset, bitmap[currentBlockID]);

    //File Read
    fseek(fp, offset, SEEK_SET);
    fread(blockptr, header.blocksize, 1, fp);

    resetFilePointerToStart(fp);
    return *blockptr;
}

int getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    SIFS_DIRBLOCK currentBlock = getDirBlockById(fp, currentBlockID);

    for (int i = 0; i < currentBlock.nentries; i++)
    {
        SIFS_BLOCKID entryblockID = currentBlock.entries[i].blockID;
        if (bitmap[entryblockID] == SIFS_DIR) //ONLY CONSIDERS DIRECTORY BLOCK
        {
            SIFS_DIRBLOCK entryDir = getDirBlockById(fp, entryblockID);
            if (strcmp(dirname, entryDir.name) == 0) //MATCHER
            {
                return entryblockID;
            }
        }
    }
    SIFS_errno = SIFS_ENOENT;
    return -1; // NON-EXISTENT DIRECTORY AT DIRECTORY
}

int getFileBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    SIFS_DIRBLOCK currentBlock = getDirBlockById(fp, currentBlockID);

    for (int i = 0; i < currentBlock.nentries; i++)
    {
        SIFS_BLOCKID entryblockID = currentBlock.entries[i].blockID;
        if (bitmap[entryblockID] == SIFS_FILE) //ONLY CONSIDERS DIRECTORY BLOCK
        {
            SIFS_FILEBLOCK entryFile = getFileBlockById(fp, entryblockID);
            uint32_t fileIndex = currentBlock.entries[i].fileindex;
            if (strcmp(filename, entryFile.filenames[fileIndex]) == 0) //MATCHER FOR FILE BLOCK AND INDEX
            {
                return entryblockID;
            }
        }
    }
    SIFS_errno = SIFS_ENOENT;
    return -1; // NON-EXISTENT FILE AT DIRECTORY
}

int getDirBlockIdBeforePathEnds(FILE *fp, const char *pathname)
{ //REVIEW  NEEDS FURTHER TESTING FOR NESTED DIRECTORY
    PATH path = getSplitPath(pathname);

    //ROOT DIRECTORY
    SIFS_BLOCKID currentBlockID = 0;

    for (int i = 0; i < path.numSubDir - 1; i++)
    {
        //ASSIGNS THE CURRENT AS THE NEXT BLOCKID IN THE SUBDIR LINE
        currentBlockID = getDirBlockIdByName(fp, currentBlockID, path.subPathArray[i]);
        if (currentBlockID < 0) //NON EXISTENT OR NONVALID DIRECTORY - occurs for bitmap f as subdir
        {
            return -1;
        }
    }
    return currentBlockID;
}

char *getPathTail(const char *pathname)
{
    PATH path = getSplitPath(pathname);
    return path.subPathArray[path.numSubDir - 1];
}
