/* CITS2002 Project 2 2019
   Name(s):             Frinze Lapuz, Thye Shan Ng
   Student number(s):   22711649, 22727425
 */

#include "sifs-internal.h"
#include "helper.h"

// remove an existing directory from an existing volume
int SIFS_rmdir(const char *volumename, const char *pathname)
{
    FILE *fp = getFileReaderPointer(volumename);
    CHECK_VOLUME_EXIST
    
    SIFS_errno = SIFS_ENOTYET;
    return 1;
}
