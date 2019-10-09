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

extern FILE *getFileReaderPointer(const char *volumename);
extern size_t fpread(FILE *fp, size_t offset, size_t start, void *buffer, size_t size, size_t nitems);

extern SIFS_VOLUME_HEADER getHeader(FILE *fp);
extern SIFS_BIT *getBitmapPtr(FILE *fp, SIFS_VOLUME_HEADER header);

//Returns pointer that points to array of pointer pointing to strings
//Can be done with char *words[number of words]
extern PATH getSplitPath(const char *pathname);
extern int getDirBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *dirname);
extern int getFileBlockIdByName(FILE *fp, SIFS_BLOCKID currentBlockID, const char *filename);

extern SIFS_DIRBLOCK getDirBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
extern SIFS_FILEBLOCK getFileBlockById(FILE *fp, SIFS_BLOCKID currentBlockID);
