/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include <stdio.h>
#include <string.h>
#include "sifs-internal.h"
#include "helper.h"

int BDataShift(FILE *fp, int UblockShift, const SIFS_BIT bitmap)
{
    //FUNCTION THAT SHIFTS DATA BY UblockShift AMOUNT TO THE LEFT 

}

int SIFS_defrag(const char *volumename)
{
    FILE *fp = getFileReaderPointer(volumename);
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    printf("%s\n",bitmap);
    int countU = 0;
    int length = strlen(bitmap);

    for (int i = 0; i < length; i++)
    {
        if (bitmap[i] == SIFS_UNUSED)
        {
            for (int j = i; j < length; j++)
            {
                if (bitmap[j] != SIFS_UNUSED)
                {
                    BDataShift(fp, countU, bitmap);
                    i = j - countU;
                    countU = 0;
                    break;
                }
                countU++;
            }
        }
    } 

    //Feel free to change (STEPS)
        /*  
            do a nested loop
            outerloop finds the first U block (lets call it X) 
                innerloop starts counting (using countU) from X until it reaches a non U block(if any)
                    innerloop will break in 2 ways
                        1. When countU = length - TheAmountOfTimesOuterLoopHasLooped
                            break both inner and outer loop     
                        OR
                        2. If non U block is found: 
                            start defragging(maybe a separate function)
                            TheAmountOfTimesOuterLoopHasLooped = TheAmountOfTimesOuterLoopHasLooped - countU
                            break
                            continue loop
                        
        */

    SIFS_errno = SIFS_ENOTYET;

    fclose(fp);
    return 0;
}

