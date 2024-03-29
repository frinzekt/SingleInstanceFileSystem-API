/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
/* helper.h
- Used for custom definition and function

*/
#include <stdio.h>
#include <stdbool.h>

#define MAX_NUM_SUBDIRECTORIES 24
#define MAX_PATH_NAME MAX_NUM_SUBDIRECTORIES *SIFS_MAX_NAME_LENGTH
#define INDEX_FAILURE -1 //CUSTOM FAILURE OF FUNCTION
#define CHECK_VOLUME_EXIST        \
    if (fp == NULL)               \
    {                             \
        SIFS_errno = SIFS_ENOVOL; \
        return EXIT_FAILURE;      \
    }

#define CHECK_VALID_VOLUME                                                                                                  \
    SIFS_VOLUME_HEADER headerCheck = getHeader(fp);                                                                         \
    SIFS_BIT *bitCheck = getBitmapPtr(fp, headerCheck);                                                                     \
    if (bitCheck[0] != SIFS_UNUSED && bitCheck[0] != SIFS_DIR && bitCheck[0] != SIFS_FILE && bitCheck[0] != SIFS_DATABLOCK) \
    {                                                                                                                       \
        SIFS_errno = SIFS_ENOTVOL;                                                                                          \
        return EXIT_FAILURE;                                                                                                \
    }

typedef struct
{
    int numSubDir;
    char subPathArray[MAX_NUM_SUBDIRECTORIES][SIFS_MAX_NAME_LENGTH];
} PATH;

//RETURNS A PATH STRUCTURE CONTAINING SUBPATH IN ARRAY THE LENGTH OF THE ARRAY
extern PATH getSplitPath(const char *pathname);
//GETS FILE READER POINTER, YOU STILL NEED TO CLOSE THE FILEPOINTER AFTER USE
extern FILE *getFileReaderPointer(const char *volumename);
extern FILE *getFileWriterPointer(const char *volumename);

//ALL THE FUNCTION REQUIRING FILE *fp will require that fp is not NULL and is open
extern SIFS_VOLUME_HEADER getHeader(FILE *fp);
extern SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header);
extern bool modifyBitmap(FILE *fp, SIFS_BIT *bitmap, SIFS_BLOCKID blockId, char bit);

//Returns -1 for NON-EXISTENT DIRECTORY, otherwise returns BlockID
extern SIFS_BLOCKID getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname);
extern SIFS_BLOCKID getFileBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename);
extern uint32_t getFileBlockIndexByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename);
//NO BITMAP CHECKING DONE HERE SO CHECK BITMAP BEFORE CALLING THIS FUNCTION
extern SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
extern SIFS_FILEBLOCK getFileBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
extern char *getBlockNameById(FILE *fp, SIFS_BLOCKID currentBlockID, uint32_t fileindex);
extern SIFS_BLOCKID getDirBlockIdBeforePathEnds(FILE *fp, const char *pathname);

extern int getOffset(FILE *fp, SIFS_BLOCKID id);

//Definitions from https://www.geeksforgeeks.org/python-os-path-split-method/
//Tail is the name at the end of the path
//Head is the pathname without the tail
extern char *getPathTail(const char *pathname);

//Unused Blocks
extern uint32_t getNoBlockRequirement(size_t length, uint32_t block_size);                                              // Converts length to number of blocks
extern SIFS_BLOCKID getNextUBlockId(SIFS_BIT *bitmap, SIFS_BLOCKID start, uint32_t nblocks);                            //RETURNS -1 on failure (if no unused)
extern SIFS_BLOCKID getNextUBlockIdWithLength(SIFS_BIT *bitmap, SIFS_BLOCKID start, int nblocks_req, uint32_t nblocks); //Calls Unused BlockID and returns -1 on failure

// RETURNS TRUE/FALSE ON SUCCESS/FAIL
// CAN FAIL IF THERE IS NOT ENOUGH LENGTH
//NOTE  dirContainerId is the blockID of where the entry(file or directory) belongs to
extern bool writeFileBlock(FILE *fp, SIFS_BLOCKID dirContainerId, const char *fileName, void *data, size_t length);
extern bool writeDataBlock(FILE *fp, SIFS_BLOCKID firstDataBlockId, void *data, int noOfBlocksNeed);
extern bool writeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId, const char *dirName);

//BLOCK REWRITER, takes ID and the new block(WITH THE NEW DETAILS) then rewrites that
extern bool modifyDirBlock(FILE *fp, SIFS_BLOCKID dirBlockId, SIFS_DIRBLOCK newDirBlock);     //HAPPENS WHEN UPDATING ENTRY AND TIME + REMOVING
extern bool modifyFileBlock(FILE *fp, SIFS_BLOCKID fileBlockId, SIFS_FILEBLOCK newFileBlock); //HAPPENS WHEN THERE'S A FILE DUPLICATE WHICH UPDATES FILENAMES and nfiles, MODIFICATION NAME??

extern bool removeBlockById(FILE *fp, SIFS_BLOCKID blockId);                                                          //REMOVES BLOCK IN FILE, NOTHING ELSE
extern bool removeFileBlockById(FILE *fp, SIFS_BLOCKID dirContainerId, SIFS_BLOCKID fileBlockId, uint32_t fileIndex); //REMOVES FILEBLOCK  IF remaining nfiles=0 ELSE removes filename
//extern bool removeDataBlock();
extern bool removeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId, SIFS_BLOCKID dirId);
