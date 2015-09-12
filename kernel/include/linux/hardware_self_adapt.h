

#ifndef HARDWARE_SELF_ADAPT_H
#define HARDWARE_SELF_ADAPT_H

#include <linux/types.h>
#include <asm/setup.h>


#define HW_VER_MAIN_MASK (0xFFF0)
#define HW_VER_SUB_MASK  (0x000F)

/*
* first byte: low 4 bits: sub version,  high 4 bits: , external version for example U8105
* high 3 bytes: base main board version
*/
typedef enum
{
    //G520
    HW_G520_VER   = 0x100,
    HW_G520_VER_A = HW_G520_VER,
    HW_G520_VER_B ,
    HW_G520_VER_C ,
    HW_G520_VER_D ,
    HW_G520_VER_E ,

    //G700
    HW_G700_VER   = 0x200,
    HW_G700_VER_A = HW_G700_VER,
    HW_G700_VER_B ,
    HW_G700_VER_C ,
    HW_G700_VER_D ,
    HW_G700_VER_E ,
    HW_G700_VER_F ,

    //G700U
    HW_G700U_VER   = 0x300,
    HW_G700U_VER_A = HW_G700U_VER,
    HW_G700U_VER_B ,
    HW_G700U_VER_C ,
    HW_G700U_VER_D ,
    HW_G700U_VER_E ,
    HW_G700U_VER_F ,

    //G610U
    HW_G610U_VER   = 0x400,
    HW_G610U_VER_A = HW_G610U_VER,
    HW_G610U_VER_B ,
    HW_G610U_VER_C ,
    HW_G610U_VER_D ,
    HW_G610U_VER_E ,
    HW_G610U_VER_F ,

    //G610T
    HW_G610_VER   = 0x500,
    HW_G610_VER_A = HW_G610_VER,
    HW_G610_VER_B ,
    HW_G610_VER_C ,
    HW_G610_VER_D ,
    HW_G610_VER_E ,
    HW_G610_VER_F ,

    /* added other type at here, it should be started 0x200 */

    HW_VER_NONE    = 0xFFFF,
}hw_product_type;

/*
* sub version type define
*/
typedef enum
{
    HW_VER_SUB_VA            = 0x0,
    HW_VER_SUB_VB            = 0x1,
    HW_VER_SUB_VC            = 0x2,
    HW_VER_SUB_VD            = 0x3,
    HW_VER_SUB_VE            = 0x4,
    HW_VER_SUB_VN            = 0x5,
    HW_VER_SUB_VO            = 0x6,
    HW_VER_SUB_VP            = 0x7,
    HW_VER_SUB_VR            = 0x8,
    HW_VER_SUB_VS            = 0x9,
    HW_VER_SUB_SURF          = 0xF,
    HW_VER_SUB_MAX           = 0xF
}hw_ver_sub_type;


/*
* app info type define
*/
typedef enum {
    BOARD_ID                 = 0x0,
    EMMC_ID                  = 0x1,
    PRIMARY_CAMERA_ID        = 0x2,
    SLAVE_CAMERA_ID          = 0x3,
    LCD_ID                   = 0x4,
    GSENSOR_ID               = 0x5,
    COMPASS_ID               = 0x6,
    TP_ID                    = 0x7,
    HW_VERSION               = 0x8,
    BATTERY_ID               = 0x9,
#ifdef CONFIG_DL_CHECK_SUPPORT
    DL_CHECK_TAG             = 0xA,
#endif
    MAX_NUM_ID
}id_type;


typedef enum
{
    BATT_ID_BYD = 0,
    BATT_ID_GAGC,
    BATT_ID_LS,
    BATT_ID_RESERVE_FOR_INLAND,
    //oversea battery
    BATT_ID_SONY,
    BATT_ID_SDI,
    BATT_ID_SANYO,
    BATT_ID_RESERVE_FOR_OVERSEA,
    BATT_ID_INVALID
}hw_battery_type;

#ifdef CONFIG_DL_CHECK_SUPPORT
typedef enum
{
    DL_PASS = 0,
    DL_FAIL,
    DL_INVALID
}hw_dl_check_tag;
#endif

#define hw_product_info_type  tag_hw_product_info
#define HW_PRODUCT_INFO_SIZE  sizeof(struct hw_product_info_type)
extern struct hw_product_info_type hw_product_info;

hw_product_type get_hardware_product_version (void);
hw_battery_type get_hardware_battery_type(void);

#ifdef CONFIG_DL_CHECK_SUPPORT
hw_dl_check_tag get_hardware_dl_check_tag(void);
#endif

#define get_hardware_product_main_version() (get_hardware_product_version() & HW_VER_MAIN_MASK)
#define get_hardware_product_sub_version()  (get_hardware_product_version() & HW_VER_SUB_MASK)


void set_id_value(id_type type, char *value);

#endif
