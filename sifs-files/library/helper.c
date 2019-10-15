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
#include <math.h>

#define START 0

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

SIFS_VOLUME_HEADER getHeader(FILE *fp)
{
    //READ HEADER
    SIFS_VOLUME_HEADER header;

    resetFilePointerToStart(fp);
    fread(&header, sizeof header, 1, fp);

    return header;
}

SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header)
{
    SIFS_BIT *bitmap = malloc(header.nblocks * sizeof(int) + 1); //REVIEW sizeof(int)

    //SKIP TO SPECIFIC BYTE COUNT THEN READ
    fseek(fp, sizeof(header), SEEK_SET);
    fread(bitmap, 1, header.nblocks, fp);

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
    return path;
}

//CHANGE ALL READ_OFFSET TO THIS ONE FUNCTION - put as an extern
int getOffset(FILE *fp, SIFS_BLOCKID id)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);

    return sizeof(header) + header.nblocks + (id) * (header.blocksize);
}

SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockId)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    //SIFS_BIT *bitmap = getBitmapPtr(fp, header); //REVIEW , Assume IS DIRBLOCK
    SIFS_DIRBLOCK *blockptr = malloc(header.blocksize + 1);

    //OFFSET... header size, bitmap size, rootdir size, sizes of previous block
    //roodir = sizes of previous block => (1+blockID-1)
    int offset = getOffset(fp, currentBlockId);
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
    SIFS_FILEBLOCK *blockptr = malloc(header.blocksize);

    //FINDING THE LOCATION
    int offset = getOffset(fp, currentBlockId);

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
        if (bitmap[entryblockID] == SIFS_DIR) //ONLY CONSIDERS DIRECTORY BLOCK FIXME : Include else if for SIFS_FILE
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
    return INDEX_FAILURE; // NON-EXISTENT DIRECTORY AT DIRECTORY
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
    return INDEX_FAILURE; // NON-EXISTENT FILE AT DIRECTORY
}

uint32_t getFileBlockIndexByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename)
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
                return fileIndex;
            }
        }
    }
    SIFS_errno = SIFS_ENOENT;
    return INDEX_FAILURE; // NON-EXISTENT FILE AT DIRECTORY
}

SIFS_BLOCKID getDirBlockIdBeforePathEnds(FILE *fp, const char *pathname)
{ //REVIEW  NEEDS FURTHER TESTING FOR NESTED DIRECTORY
    PATH path = getSplitPath(pathname);
    SIFS_BLOCKID currentBlockID = SIFS_ROOTDIR_BLOCKID;

    //LOOP BEFORE THE END, NOT INCLUDING END
    for (int i = 0; i < path.numSubDir - 1; i++)
    {
        //ASSIGNS THE CURRENT AS THE NEXT BLOCKID IN THE SUBDIR LINE
        currentBlockID = getDirBlockIdByName(fp, currentBlockID, path.subPathArray[i]);
        if (currentBlockID < SIFS_ROOTDIR_BLOCKID) //NON EXISTENT OR NONVALID DIRECTORY - occurs for bitmap f as subdir
        {
            SIFS_errno = SIFS_ENOENT;
            return INDEX_FAILURE;
        }
    }
    return currentBlockID;
}

char *getPathTail(const char *pathname)
{
    PATH path = getSplitPath(pathname);
    char *tailptr = malloc(SIFS_MAX_NAME_LENGTH);
    strcpy(tailptr, path.subPathArray[path.numSubDir - 1]);
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
        strcpy(name, getDirBlockById(fp, currentBlockID).name);
    }
    else if (IsFile)
    {
        strcpy(name, getFileBlockById(fp, currentBlockID).filenames[fileindex]);
    }
    else
    {
        printf("WARNING: DATA BLOCK OR UNUSED");
        return NULL;
    }
    return name;
}

uint32_t getNoBlockRequirement(size_t length, uint32_t block_size)
{
    //REVIEW NEEDS TESTING
    return ceil(length * 1.0 / block_size);
}

SIFS_BLOCKID getNextUBlockId(SIFS_BIT *bitmap, SIFS_BLOCKID start)
{
    //REVIEW NEEDS TESTING
    int length = strlen(bitmap);
    int i;
    for (i = start; i < length; i++)
    {
        if (bitmap[i] == SIFS_UNUSED)
        {
            return i;
        }
    }
    SIFS_errno = SIFS_ENOSPC;
    return INDEX_FAILURE;
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
                len_ubit = 0;
                break;
            }
        }

    } while (start != INDEX_FAILURE);
    return INDEX_FAILURE;
}

bool modifyBitmap(FILE *fp, SIFS_BIT *bitmap, SIFS_BLOCKID blockId, char bit)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    bitmap[blockId] = bit;
    fseek(fp, sizeof(header), SEEK_SET);
    fwrite(bitmap, header.nblocks, 1, fp);
    return true;
}

bool modifyDirBlock(FILE *fp, SIFS_BLOCKID currentBlockId, SIFS_DIRBLOCK newBlock)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    newBlock.modtime = time(NULL);
    int offset = getOffset(fp, currentBlockId);
    fseek(fp, offset, SEEK_SET);
    fwrite(&newBlock, header.blocksize, 1, fp);

    return true;
}

bool modifyFileBlock(FILE *fp, SIFS_BLOCKID currentBlockId, SIFS_FILEBLOCK newBlock)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    newBlock.modtime = time(NULL);
    int offset = getOffset(fp, currentBlockId);
    fseek(fp, offset, SEEK_SET);
    fwrite(&newBlock, header.blocksize, 1, fp);

    return true;
}

bool checkName(FILE *fp, SIFS_BLOCKID dirContainerId, const char *name)
{
    SIFS_DIRBLOCK dirContainer = getDirBlockById(fp, dirContainerId);
    for (int i = 0; i < dirContainer.nentries; i++)
    {
        SIFS_BLOCKID currentBlockId = dirContainer.entries[i].blockID;
        uint32_t currentFileIndex = dirContainer.entries[i].fileindex;
        printf("NAME COMPARE: %s and %s\n", getBlockNameById(fp, currentBlockId, currentFileIndex), name);
        if (strcmp(getBlockNameById(fp, currentBlockId, currentFileIndex), name) == 0)
        {
            return false;
        }
    }
    return true;
}

bool writeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId, const char *dirName)
{
    //ANCHOR Errno DONE IN THIS FUNCTION
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    SIFS_BLOCKID currentBlockId = getNextUBlockId(bitmap, START);

    //ERROR CHECK
    if (currentBlockId == INDEX_FAILURE)
    {
        SIFS_errno = SIFS_ENOSPC;
        return false;
    }

    if (!checkName(fp, dirContainerId, dirName))
    {
        SIFS_errno = SIFS_EEXIST;
        return false;
    }
    //REWRITE BITMAP
    modifyBitmap(fp, bitmap, currentBlockId, SIFS_DIR);

    //WRITE NEW BLOCK

    int offset = getOffset(fp, currentBlockId);
    SIFS_DIRBLOCK block = {
        .modtime = time(NULL),
        .nentries = 0,
    };

    strcpy(block.name, dirName);
    fseek(fp, offset, SEEK_SET);
    fwrite(&block, header.blocksize, 1, fp);

    //ADD AS NEW ENTRY IN CONTAINER
    SIFS_DIRBLOCK container = getDirBlockById(fp, dirContainerId);
    container.entries[container.nentries].blockID = currentBlockId;
    container.entries[container.nentries].fileindex = 0;
    container.nentries++;

    modifyDirBlock(fp, dirContainerId, container);

    return true;
}

bool writeDataBlock(FILE *fp, SIFS_BLOCKID firstDataBlockId, void *data, int noOfBlocksNeed)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);

    //Update Bitmap
    for (SIFS_BLOCKID i = 0; i < noOfBlocksNeed; i++)
    {
        modifyBitmap(fp, bitmap, firstDataBlockId + i, SIFS_DATABLOCK);
    }

    //WRITE TO FILE
    int offset = getOffset(fp, firstDataBlockId);
    fseek(fp, offset, SEEK_SET);
    fwrite(data, header.blocksize * noOfBlocksNeed, 1, fp);
    return true;
}

bool writeFileBlock(FILE *fp, SIFS_BLOCKID dirContainerId, const char *fileName, void *data, size_t length)
{
    //ANCHOR Errno DONE IN THIS FUNCTION
    bool IsDuplicate = false;
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);

    int noRequiredBlocks = getNoBlockRequirement(length, header.blocksize);

    SIFS_BLOCKID currentBlockId = 0;
    SIFS_BLOCKID firstDataBlockId = 0;
    SIFS_BLOCKID fileBlockId = 0;
    uint32_t fileIndex = 0;

    if (!checkName(fp, dirContainerId, fileName))
    {
        SIFS_errno = SIFS_EEXIST;
        return false;
    }

    //MD5 Calculation
    unsigned char *md5 = malloc(MD5_BYTELEN + 1);
    char *md5_str = malloc(MD5_STRLEN + 1);
    MD5_buffer(data, length, md5);
    strcpy(md5_str, MD5_format(md5));

    //DUPLICATE OR NOT ?
    for (SIFS_BLOCKID i = 0; i < header.nblocks; i++)
    {
        if (bitmap[i] == SIFS_FILE) //ITERATE OVER ALL FILES
        {

            SIFS_FILEBLOCK block = getFileBlockById(fp, i);
            if (strcmp(md5_str, MD5_format(block.md5)) == 0)
            { //IT IS A DUPLICATE - JUST ADD NAME IN FILEBLOCK
                IsDuplicate = true;
                fileBlockId = i;
                fileIndex = block.nfiles;

                strcpy(block.filenames[block.nfiles], fileName);
                block.nfiles++;

                modifyFileBlock(fp, fileBlockId, block);
                break;
            }
        }
    }
    if (!IsDuplicate) //GET IDs and Spaces
    {
        currentBlockId = getNextUBlockId(bitmap, START);
        firstDataBlockId = getNextUBlockIdWithLength(bitmap, currentBlockId + 1, noRequiredBlocks);

        //SPACE ERROR CHECK
        if ((currentBlockId == INDEX_FAILURE) || (firstDataBlockId == INDEX_FAILURE))
        {
            SIFS_errno = SIFS_ENOSPC;
            return false;
        }
    }

    //ADD AS NEW ENTRY IN CONTAINER
    SIFS_DIRBLOCK container = getDirBlockById(fp, dirContainerId);
    container.entries[container.nentries].blockID = (IsDuplicate) ? fileBlockId : currentBlockId;
    container.entries[container.nentries].fileindex = (IsDuplicate) ? fileIndex : 0;
    container.nentries++;
    modifyDirBlock(fp, dirContainerId, container);

    if (IsDuplicate)
    {
        return true;
    }
    else //CREATE FILE BLOCK AND DATA BLOCK
    {
        SIFS_FILEBLOCK block = {
            .modtime = time(NULL),
            .length = length,
            .firstblockID = firstDataBlockId,
            .nfiles = 1,
        };
        memcpy(block.md5, md5, MD5_BYTELEN);
        strcpy(block.filenames[0], fileName);
        printf("IS SEG FAULT HERE?? \n");
        int offset = getOffset(fp, currentBlockId);
        fseek(fp, offset, SEEK_SET);
        fwrite(&block, header.blocksize, 1, fp);
        modifyBitmap(fp, bitmap, currentBlockId, SIFS_FILE);
        writeDataBlock(fp, firstDataBlockId, data, noRequiredBlocks);
    }

    return true;
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

    modifyBitmap(fp, bitmap, blockId, SIFS_UNUSED);

    return true;
}

bool removeFileBlockById(FILE *fp, SIFS_BLOCKID dirContainerId, SIFS_BLOCKID fileBlockId, uint32_t fileIndex)
{ //REMOVES ONLY IF THERE IS ONLY ONE INSTANCE OF FILE,
    //REVIEW NEEDS TESTING
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    SIFS_DIRBLOCK container = getDirBlockById(fp, dirContainerId);
    SIFS_FILEBLOCK target = getFileBlockById(fp, fileBlockId);

    if (bitmap[fileBlockId] != SIFS_FILE || fileBlockId > strlen(bitmap))
    {
        //ANCHOR Errno DONE IN THIS FUNCTION
        SIFS_errno = SIFS_ENOTFILE;
        printf("...This is not a file...\n");
        return false;
    }
    else if (target.nfiles < fileIndex + 1)
    {
        SIFS_errno = SIFS_ENOENT;
        printf("No such file exists\n");
        return false;
    }
    int no_block = getNoBlockRequirement(target.length, header.blocksize);

    if (target.nfiles == 1)
    {
        //if number of files in target is = 1 then deletes all the b blocks and file block id
        for (int i = target.firstblockID; i < (no_block + target.firstblockID); i++)
        {
            removeBlockById(fp, i);
        }
        removeBlockById(fp, fileBlockId);
    }
    else
    {
        //if there's more file
        for (int i = fileIndex; i < (target.nfiles - fileIndex); i++)
        {
            //REVIEW not sure about this function, but it basically shifts the file name up by 1 fileindex
            //eg. filename[1] = "", filename [2] = "file", -> filename[1] = "file"
            strcpy(target.filenames[i], target.filenames[i + 1]);
        }
        target.nfiles--;
        modifyFileBlock(fp, fileBlockId, target);
    }

    for (int i = 0; i < container.nentries; i++)
    {
        //Same loop as removedirblock
        SIFS_BLOCKID contentId = container.entries[i].blockID;
        uint32_t contentIndex = container.entries[i].fileindex;
        if (contentId == fileBlockId)
        {
            if (contentIndex == fileIndex)
            {
                for (int j = i; j < container.nentries; j++)
                {
                    container.entries[j].blockID = container.entries[j + 1].blockID;
                    container.entries[j].fileindex = container.entries[j + 1].fileindex;
                }
                container.nentries--;
            }
            else if (contentIndex > fileIndex)
            { // SHIFTING FILE INDEX IN THE DIRECTORY IF IT IS FURTHER
                container.entries[i].fileindex--;
            }
        }
    }
    modifyDirBlock(fp, dirContainerId, container);
    return true;
}

bool removeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId, SIFS_BLOCKID dirId)
{ // calls function to remove directory ... calls modify function to modify container
    //dircontainerid shows blockid of files
    //ANCHOR Errno DONE IN THIS FUNCTION
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    SIFS_DIRBLOCK container = getDirBlockById(fp, dirContainerId);
    SIFS_DIRBLOCK target = getDirBlockById(fp, dirId);

    //ERROR CHECKING
    if (bitmap[dirId] != SIFS_DIR || dirId > strlen(bitmap))
    {
        SIFS_errno = SIFS_ENOTDIR;
        printf("...This is not a directory...\n");
        return false;
    }
    else if (target.nentries != 0)
    {
        SIFS_errno = SIFS_ENOTEMPTY;
        printf("...CANNOT DELETE, HAS ITEMS...\n");
        return false;
    }
    //getDirBlockById(fp,dirContainerId).entries[i].blockID

    //MODIFY THE CONTAINER
    for (int i = 0; i < container.nentries; i++)
    {
        SIFS_BLOCKID contentId = container.entries[i].blockID;
        if (contentId == dirId)
        {
            for (int j = i; j < container.nentries; j++)
            {
                container.entries[j].blockID = container.entries[j + 1].blockID;
                container.entries[j].fileindex = container.entries[j + 1].fileindex;
            }
            container.nentries--;
        }
    }
    modifyDirBlock(fp, dirContainerId, container);

    //DELETE
    removeBlockById(fp, dirId);

    return true;
}
