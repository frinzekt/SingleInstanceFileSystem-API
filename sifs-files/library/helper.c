/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

FILE *getFileReaderPointer(const char *volumename)
{
    FILE *fp = fopen(volumename, "rb+");
    if (fp == NULL)
    {
        //READING ERROR
        SIFS_errno = SIFS_ENOVOL;
        return NULL;
    }
    return fp;
}
FILE *getFileWriterPointer(const char *volumename)
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
void resetFilePointerToStart(FILE *fp) //REVIEW
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

    //FSEEK
    resetFilePointerToStart(fp);
    fread(&header, sizeof header, 1, fp);
    printf("blocksize=%i,  nblocks=%i\n", (int)header.blocksize, (int)header.nblocks);

    resetFilePointerToStart(fp);
    return header;
}

SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header)
{
    SIFS_BIT *bitmap = malloc(header.nblocks * sizeof(int) + 1); //REVIEW sizeof(int)

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
    printf("}\n");
    return path;
}

#define READ_OFFSET sizeof(header) + header.nblocks + (currentBlockId) * (header.blocksize)

SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockId)
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
SIFS_FILEBLOCK getFileBlockById(FILE *fp, SIFS_BLOCKID currentBlockId)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    // SIFS_BIT *bitmap = getBitmapPtr(fp, header); //REVIEW , Assume IS FILEBLOCK
    SIFS_FILEBLOCK *blockptr = malloc(header.blocksize);
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

SIFS_BLOCKID getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname)
{
    printf("COMPARING: %s ----\n", dirname); //FIXME
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    SIFS_DIRBLOCK currentBlock = getDirBlockById(fp, currentBlockID);

    for (int i = 0; i < currentBlock.nentries; i++)
    {

        SIFS_BLOCKID entryblockID = currentBlock.entries[i].blockID;
        if (bitmap[entryblockID] == SIFS_DIR) //ONLY CONSIDERS DIRECTORY BLOCK
        {
            SIFS_DIRBLOCK entryDir = getDirBlockById(fp, entryblockID);
            printf("COMPARING: %s and %s\n", dirname, entryDir.name);
            if (strcmp(dirname, entryDir.name) == 0) //MATCHER
            {
                return entryblockID;
            }
        }
    }
    SIFS_errno = SIFS_ENOENT;
    return -1; // NON-EXISTENT DIRECTORY AT DIRECTORY
}

SIFS_BLOCKID getFileBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename)
{ //REVIEW  NEEDS TESTING
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

SIFS_BLOCKID getDirBlockIdBeforePathEnds(FILE *fp, const char *pathname)
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
    char *tailptr = path.subPathArray[path.numSubDir - 1];
    return tailptr;
}

char *getBlockNameById(FILE *fp, SIFS_BLOCKID currentBlockID, uint32_t fileindex)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);

    char *name = malloc(SIFS_MAX_NAME_LENGTH);

    bool IsDir = (bitmap[currentBlockID] == SIFS_DIR);
    bool IsFile = (bitmap[currentBlockID] == SIFS_FILE);
    //printf("%s\n", bitmap);
    //printf("BITMAP:%c, ISDIR: %i, ISFILE %i\n", bitmap[currentBlockID], IsDir, IsFile);
    if (IsDir)
    {
        name = getDirBlockById(fp, currentBlockID).name;
    }
    else if (IsFile)
    {
        name = getFileBlockById(fp, currentBlockID).filenames[fileindex];
    }
    else
    {
        printf("WARNING: DATA BLOCK OR UNUSED");
        return NULL;
    }
    return name;
}

int getNoBlockRequirement(size_t length, uint32_t block_size)
{
    //REVIEW NEEDS TESTING
    return ceil(length / block_size);
}

SIFS_BLOCKID getNextUBlockId(SIFS_BIT *bitmap, SIFS_BLOCKID start)
{
    //REVIEW NEEDS TESTING
    int length = strlen(bitmap);
    int i = start;
    for (i; i < length; i++)
    {
        if (bitmap[i] == SIFS_UNUSED)
        {
            return i;
        }
    }
    return -1;
}

SIFS_BLOCKID getNextUBlockIdWithLength(SIFS_BIT *bitmap, SIFS_BLOCKID start, int nblocks_req)
{
    //REVIEW NEEDS TESTING
    int len_ubit = 0;
    do
    {
        start = getNextUBlockId(bitmap, start);
        for (int i = start; i < strlen(bitmap); i++)
        {
            if (bitmap[i] == SIFS_UNUSED)
            {
                len_ubit++;
                if (len_ubit == nblocks_req)
                {
                    return start;
                }
            }
            else
            {
                break;
            }
            
        }

    } while (start != -1);
    return -1;
}

char *getVolPath(const char*volumename)  //eg. for sample/Vold you get sample/
{
    //ANCHOR  MIGHT DELETE THIS IN FUTURE, leaving it here for now
    char *current_path = malloc(sizeof(char) * PATH_MAX);
    PATH vol_path = getSplitPath(volumename);
    for (int i = 0; i < (vol_path.numSubDir - 1); i++)
    {
        strcat(current_path, vol_path.subPathArray[i]);
        strcat(current_path, "/");
    }
    //printf("%s\n", current_path);
    return current_path;
}
bool modifyDirBlock(FILE *fp, SIFS_BLOCKID currentBlockId, SIFS_DIRBLOCK newBlock)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);

    int offset = READ_OFFSET;
    fseek(fp, offset, SEEK_SET);
    fwrite(&newBlock, header.blocksize, 1, fp);

    return true;
}
bool modifyFileBlock(FILE *fp, SIFS_BLOCKID currentBlockId, SIFS_FILEBLOCK newBlock)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);

    int offset = READ_OFFSET;
    fseek(fp, offset, SEEK_SET);
    fwrite(&newBlock, header.blocksize, 1, fp);

    return true;
}

bool writeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId, const char *dirName)
{ //TODO
    SIFS_VOLUME_HEADER header = getHeader(fp);
    //ERROR CHECKING
    SIFS_DIRBLOCK block = {
        .name = dirName,
        .modtime = time(NULL),
        .nentries = 0,
    };
    fseek(fp, 0, SEEK_END);
    fwrite(&block, header.blocksize, 1, fp);
    return true;
}

bool removeFileBlockById(FILE *fp, SIFS_BLOCKID dirContainerId, SIFS_BLOCKID fileBlockId, uint32_t fileIndex)
{
    return false;
}

bool removeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId, SIFS_BLOCKID dirId)
{
    return false;
}


bool removeBlockById(FILE *fp, SIFS_BLOCKID blockId)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);

    if (bitmap[blockId] == SIFS_UNUSED || blockId > strlen(bitmap))
    {
        printf("...Nothing to remove...\n");
        return false;
    }
    
    /*fseek(fp,1,blockId);
    fputs("u", fp);
    resetFilePointerToStart(fp);*/
    

    //fclose(volumename);
    //remove(volumename);
    //rename(cpypath,volumename);
    //free(volpath);
    return false;
}
