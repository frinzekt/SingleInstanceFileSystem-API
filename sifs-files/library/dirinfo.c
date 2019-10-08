/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include <stdio.h>
#include <time.h>

#include "sifs-internal.h"
#include "helper.h"

char file_arr[24][32];              //file, or rather variable that stores directory path name
char (*file_ptr)[32] = file_arr;    //pointer to file array

// void info_print(void)
// {
//     //printf("contents of blockID = %i:\n\n", /*number*/);
//     printf("struct {\n");
//     //printf("\t\tname = \"%c\"\n", /*dirname*/);
//     //printf("modified = %i (%c)\n", /*unix*/ /*actual date n time*/);
//     // printf("nentries = %i\n\n", /*entry no*/);

//     for (int i = 0; i < SIFS_MAX_ENTRIES; i++)
//     {
//         //printf("\tentries[%2i] = %c\n", /*block file index*/)
//     }

//     printf("} SIFS_DIRBLOCK;\n");
// }

void blockID(char *path)
{
    //NOT SURE WHAT TO RENAME IT, BUT FOR NOW blockID, THIS FUNCTION SPLITS path name via '/' separately
    char *token;

    token = strtok(path, "/");

    while (token != NULL)
    {
        strcpy(*file_ptr, token);
        file_ptr++;
        token = strtok(NULL, "/");
    }

}
// get information about a requested directory
int SIFS_dirinfo(const char *volumename, const char *pathname,
                 char ***entrynames, uint32_t *nentries, time_t *modtime)
{
    //FIXME  FOR NOW ASSUME PATHNAME IS from root directory
    /*  SIFS_DIRBLOCK dir = {
        .name = }*/
    blockID(pathname);
    SIFS_errno = SIFS_ENOTYET;
    return 1;
}
