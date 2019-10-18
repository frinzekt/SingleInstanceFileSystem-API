/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include <stdio.h>
#include <string.h>
#include "sifs-internal.h"
#include "helper.h"

//coalesces all unused blocks into a single contiguous area at the end of the volume's third section

SIFS_BLOCKID getDataBlockOwner(FILE *fp, SIFS_BLOCKID dataBlockId, SIFS_BIT *bitmap, uint32_t nblocks)
{
    for (int i = 0; i < nblocks; i++)
    {
        //ITERATE OVER ALL FILEBLOCKS
        if (bitmap[i] == SIFS_FILE)
        {
            SIFS_FILEBLOCK block = getFileBlockById(fp, i);
            if (block.firstblockID == dataBlockId)
            {
                return i;
            }
        }
    }
    return INDEX_FAILURE; //unlikely to happen if there is a correct reading
}

SIFS_BLOCKID getEntryIndex(FILE *fp, SIFS_BLOCKID containerId, SIFS_BLOCKID entryId, uint32_t fileIndex)
{
    SIFS_DIRBLOCK container = getDirBlockById(fp, containerId);
    for (int i = 0; i < container.nentries; i++)
    {
        if ((container.entries[i].blockID == entryId) && (container.entries[i].fileindex == fileIndex))
        {
            return i;
        }
    }
    return INDEX_FAILURE; //CANT FIND ENTRY AND FILEINDEX IN CONTAINER
}

SIFS_BLOCKID getContainerId(FILE *fp, SIFS_BLOCKID entryID, uint32_t fileIndex, SIFS_BIT *bitmap, uint32_t nblocks)
{
    for (int i = 0; i < nblocks; i++) // ITERATES OVER ALL DIRECTORIES
    {
        if (bitmap[i] == SIFS_DIR)
        {

            if (getEntryIndex(fp, i, entryID, fileIndex) != INDEX_FAILURE)
            {
                //CHECK IF THE ENTRY EXIST IN THE DIR/container
                return i;
            }
        }
    }
    return INDEX_FAILURE; //unlikely
}

bool BDataShift(FILE *fp, SIFS_BLOCKID dataBlockId, int UBlockCount, SIFS_BIT *bitmap)
{
    SIFS_VOLUME_HEADER header = getHeader(fp);
    //DATA BLOCK SHIFTING - Datablock
    //FUNCTION THAT SHIFTS DATA BY UblockShift AMOUNT TO THE LEFT
    SIFS_BLOCKID fileBlockId = getDataBlockOwner(fp, dataBlockId, bitmap, header.nblocks);
    SIFS_FILEBLOCK fileBlock = getFileBlockById(fp, fileBlockId);
    SIFS_BLOCKID firstUBlock = dataBlockId - UBlockCount;

    //READ DATA BLOCK
    void *data = malloc(fileBlock.length + 1);
    fseek(fp, getOffset(fp, fileBlock.firstblockID), SEEK_SET);
    fread(data, fileBlock.length, 1, fp);

    //UPDATE FILE BLOCK
    int noOfDatablocks = getNoBlockRequirement(fileBlock.length, header.blocksize);
    fileBlock.firstblockID = firstUBlock;
    //REWRITE FILE BLOCK
    modifyFileBlock(fp, fileBlockId, fileBlock);

    //REWRITE DATA BLOCK
    fseek(fp, getOffset(fp, firstUBlock), SEEK_SET);
    fwrite(data, fileBlock.length, 1, fp);

    //REWRITE BITMAP
    for (int i = 0; i < noOfDatablocks; i++)
    {
        modifyBitmap(fp, bitmap, firstUBlock + i, SIFS_DATABLOCK); //TURN TO DATA BLOCK
        modifyBitmap(fp, bitmap, dataBlockId + i, SIFS_UNUSED);    //TURN TO UBLOCK
    }
    return false; 
}

bool entryShift(FILE *fp, SIFS_BLOCKID entryBlockId, int UBlockCount, SIFS_BIT *bitmap)
{
    //DIRECTORY AND FILE SHIFTING
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BLOCKID containerId = INDEX_FAILURE; // Initialization
    SIFS_DIRBLOCK container;
    SIFS_BLOCKID firstUBlock = entryBlockId - UBlockCount;

    if (bitmap[entryBlockId] == SIFS_DIR)
    {
        SIFS_DIRBLOCK dirBlock = getDirBlockById(fp, entryBlockId);
        containerId = getContainerId(fp, entryBlockId, 0, bitmap, header.nblocks);
        container = getDirBlockById(fp, containerId);

        // FIND THE ENTRY NUMBER
        uint32_t entryIndex = getEntryIndex(fp, containerId, entryBlockId, 0);

        container.entries[entryIndex].blockID = firstUBlock;
        modifyDirBlock(fp, containerId, container);
        modifyDirBlock(fp, firstUBlock, dirBlock);
        modifyBitmap(fp, bitmap, firstUBlock, SIFS_DIR); //TURN TO DATA BLOCK
    }
    else
    { //FILE BLOCK
        SIFS_FILEBLOCK fileBlock = getFileBlockById(fp, entryBlockId);
        modifyFileBlock(fp, firstUBlock, fileBlock);
        modifyBitmap(fp, bitmap, firstUBlock, SIFS_FILE); //TURN TO =FILE BLOCK
        for (int i = 0; i < fileBlock.nfiles; i++)
        {
            containerId = getContainerId(fp, entryBlockId, i, bitmap, header.nblocks);
            container = getDirBlockById(fp, containerId);

            // FIND THE ENTRY NUMBER
            uint32_t entryIndex = getEntryIndex(fp, containerId, entryBlockId, i);

            container.entries[entryIndex].blockID = firstUBlock;

            modifyDirBlock(fp, containerId, container); //UPDATE MULTIPLE CONTAINER
        }
    }

    //REWRITE BITMAP

    modifyBitmap(fp, bitmap, entryBlockId, SIFS_UNUSED); //TURN TO UBLOCK

    return false; //unlikely
}

int SIFS_defrag(const char *volumename)
{
    FILE *fp = getFileWriterPointer(volumename);
    CHECK_VOLUME_EXIST
    SIFS_VOLUME_HEADER header = getHeader(fp);
    SIFS_BIT *bitmap = getBitmapPtr(fp, header);
    int countU = 0;
    int length = header.nblocks;

    for (int i = 0; i < length; i++)
    {
        if (bitmap[i] == SIFS_UNUSED)
        {
            for (int j = i; j < length; j++)
            {
                if (bitmap[j] == SIFS_DATABLOCK)
                {
                    //FIND DATA BLOCK WHO OWNS DATABLOCK
                    BDataShift(fp, j, countU, bitmap);
                    i = j - countU;                    //SKIPPING TO THE NEXT UNUSED
                    bitmap = getBitmapPtr(fp, header); //UPDATE NEW BITMAP

                    break;
                }
                else if ((bitmap[j] == SIFS_DIR) || (bitmap[j] == SIFS_FILE))
                {
                    //FIND CONTAINER
                    entryShift(fp, j, countU, bitmap);
                    bitmap = getBitmapPtr(fp, header); //UPDATE NEW BITMAP

                    break;
                }
                countU++; //CALCULATING GAP OF USED AND UNUSED
            }
            countU = 0;
        }
    }

    SIFS_errno = SIFS_ENOTYET;

    fclose(fp);
    return EXIT_SUCCESS;
}
