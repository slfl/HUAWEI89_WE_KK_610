#include <linux/types.h>
#include <cust_acc.h>
#include <mach/mt_pm_ldo.h>
#include <linux/hardware_self_adapt.h>


/*---------------------------------------------------------------------------*/
static struct acc_hw cust_acc_hw = {
    .i2c_num = 3,
    .direction = 2,
    .power_id = MT65XX_POWER_NONE,  /*!< LDO is not used */
    .power_vol= VOL_DEFAULT,        /*!< LDO is not used */
    .firlen = 0, //old value 16                /*!< don't enable low pass fileter */
};
/*---------------------------------------------------------------------------*/
struct acc_hw* lis3dh_get_cust_acc_hw(void) 
{
    hw_product_type boardType = get_hardware_product_version();
    if((boardType & HW_VER_MAIN_MASK) == HW_G700U_VER)
        cust_acc_hw.direction=2;
    else if((boardType & HW_VER_MAIN_MASK) == HW_G610U_VER)
        cust_acc_hw.direction=3;
    else
    {
        cust_acc_hw.direction = 2;	/*use the default direction*/
    }
    return &cust_acc_hw;
}
