/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
/* helper.h
- Used for custom definition and function

*/
#include <stdio.h>

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

// NOTE  ALL THE FUNCTION REQUIRING FILE *fp will require that fp is not NULL and is open
extern SIFS_VOLUME_HEADER getHeader(FILE *fp);
extern SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header);

//Returns -1 for NON-EXISTENT DIRECTORY, otherwise returns BlockID
extern int getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname);
extern int getFileBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename);

extern SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
extern SIFS_FILEBLOCK getFileBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
