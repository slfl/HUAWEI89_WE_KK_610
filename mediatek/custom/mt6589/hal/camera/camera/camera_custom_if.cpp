#include <stdlib.h>
#include <stdio.h>
#include "camera_custom_if.h"
#include <cutils/properties.h>  // For property_get()

namespace NSCamCustom
{
/*******************************************************************************
* 
*******************************************************************************/


/*******************************************************************************
* custom exif
*******************************************************************************/
#define EN_CUSTOM_EXIF_INFO
MINT32 custom_SetExif(void **ppCustomExifTag)
{
#ifdef EN_CUSTOM_EXIF_INFO
#define CUSTOM_EXIF_STRING_MAKE  "HUAWEI"
#define CUSTOM_EXIF_STRING_MODEL "G700-U00"
#define CUSTOM_EXIF_STRING_SOFTWARE "G700-U00 V100R001CHNC01"
static customExifInfo_t exifTag = {CUSTOM_EXIF_STRING_MAKE,CUSTOM_EXIF_STRING_MODEL,CUSTOM_EXIF_STRING_SOFTWARE};
property_get("ro.product.manufacturer", (char*)(exifTag.strMake), CUSTOM_EXIF_STRING_MAKE);
property_get("ro.product.model", (char*)(exifTag.strModel), CUSTOM_EXIF_STRING_MODEL);
property_get("ro.build.display.id", (char*)(exifTag.strSoftware), CUSTOM_EXIF_STRING_SOFTWARE);
    if (0 != ppCustomExifTag) {
        *ppCustomExifTag = (void*)&exifTag;
    }
    return 0;
#else
    return -1;
#endif
}
//
customExif_t const&
getCustomExif()
{
    static customExif_t inst = {
        bEnCustom       :   false,  // default value: false.
        u4ExpProgram    :   0,      // default value: 0.    '0' means not defined, '1' manual control, '2' program normal
    };
    return inst;
}
//
MINT32 get_atv_disp_delay(MINT32 mode)
{
    return ((ATV_MODE_NTSC == mode)?ATV_MODE_NTSC_DELAY:((ATV_MODE_PAL == mode)?ATV_MODE_PAL_DELAY:0));
}

MINT32 get_atv_input_data()
{
    return ATV_INPUT_DATA_FORMAT;
}

/*******************************************************************************
* 
*******************************************************************************/
};  //NSCamCustom

