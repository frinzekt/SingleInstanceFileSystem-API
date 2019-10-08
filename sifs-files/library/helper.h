/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */
/* helper.h
- Used for custom definition and function

*/

//Returns pointer that points to array of pointer pointing to strings
//Can be done with char *words[number of words]
extern char **getSplitPath(const char *pathname);
extern int getDirBlockIdByName(int currentBlockID, const char *dirname);
extern int getFileBlockIdByName(int currentBlockID, const char *filename);

extern SIFS_DIRBLOCK getDirBlockById(int currentBlockID);
extern SIFS_FILEBLOCK getFileBlockById(int currentBlockID);
