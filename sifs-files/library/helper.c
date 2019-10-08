/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"
#include <stdio.h>

SIFS_VOLUME_HEADER getHeader(const char *volumename)
{
    printf("%s\n", volumename);
    FILE *fp = fopen(volumename, "r+");
    if (fp == NULL)
    {
        //READING ERROR
        printf("READING ERROR\n");
    }
    //READ HEADER
    SIFS_VOLUME_HEADER header;
    fread(&header, sizeof header, 1, fp);
    printf("blocksize=%i,  nblocks=%i\n", (int)header.blocksize, (int)header.nblocks);

    fclose(fp);
    return header;
}

SIFS_BIT *getBitmapPtr(const char *volumename, SIFS_VOLUME_HEADER header)
{
    printf("%s\n", volumename);
    FILE *fp = fopen(volumename, "r+");

    if (fp == NULL)
    {
        //READING ERROR
        printf("READING ERROR\n");
    }
    SIFS_BIT *bitmap = malloc(header.nblocks * sizeof(int) + 1);

    //SKIP TO SPECIFIC BYTE COUNT THEN READ
    fseek(fp, sizeof header, SEEK_SET);
    fread(bitmap, 1, header.nblocks, fp);
    printf("%s %d\n", bitmap, (int)(header.nblocks));

    fclose(fp);
    return bitmap;
}

int getDirBlockIdByName(const char *volumename, int currentBlockID, const char *dirname)
{
    // REVIEW
    printf("CBID: %d, Dirname: %s\n", currentBlockID, dirname);
    return 0;
}
