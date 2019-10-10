/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
/* helper.h
- Used for custom definition and function

*/
#include <stdio.h>
#include <stdbool.h>

#define MAX_NUM_SUBDIRECTORIES 24 //REVIEW  CHANGE THIS

typedef struct
{
    int numSubDir;
    char subPathArray[MAX_NUM_SUBDIRECTORIES][SIFS_MAX_NAME_LENGTH];
} PATH;

//Returns a path structure containing subpath in Array the length of the array
extern PATH getSplitPath(const char *pathname);
//GETS FILE READER POINTER, you still need to close the filepointer after use
extern FILE *getFileReaderPointer(const char *volumename);
extern FILE *getFileWriterPointer(const char *volumename); //FIXME

// NOTE  ALL THE FUNCTION REQUIRING FILE *fp will require that fp is not NULL and is open
extern SIFS_VOLUME_HEADER getHeader(FILE *fp);
extern SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header);

//Returns -1 for NON-EXISTENT DIRECTORY, otherwise returns BlockID
extern SIFS_BLOCKID getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname);
extern SIFS_BLOCKID getFileBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename);

//NO BITMAP CHECKING DONE HERE SO CHECK BITMAP BEFORE CALLING THIS FUNCTION
extern SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
extern SIFS_FILEBLOCK getFileBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
extern char *getBlockNameById(FILE *fp, SIFS_BLOCKID currentBlockID, uint32_t fileindex);
// eg. a/b/c.h
// a/b is the head
// b is the last head
extern SIFS_BLOCKID getDirBlockIdBeforePathEnds(FILE *fp, const char *pathname);

//Definitions from https://www.geeksforgeeks.org/python-os-path-split-method/
//Tail is the name at the end of the path
//Head is the pathname without the tail
extern char *getPathTail(const char *pathname);

//Unused Blocks
//FIXME
extern int getNoBlockRequirement(size_t length, uint32_t nblocks_header);                                  // Converts length to number of blocks
extern SIFS_BLOCKID getNextUnusedBlockId(SIFS_BIT *bitmap, SIFS_BLOCKID start);                            //RETURNS -1 on failure
extern SIFS_BLOCKID getNextUnusedBlockIdWithLength(SIFS_BIT *bitmap, SIFS_BLOCKID start, int nblocks_req); //Calls Unused BlockID and returns -1 on failure

// RETURNS TRUE/FALSE ON SUCCESS/FAIL
// CAN FAIL IF THERE IS NOT ENOUGH LENGTH
//FIXME
extern bool writeFileBlock(FILE *fp, SIFS_BLOCKID dirContainerId);
//extern bool writeDataBlock(FILE *fp, );
extern bool writeDirBlock(FILE *fp, SIFS_BLOCKID dirContainerId);
