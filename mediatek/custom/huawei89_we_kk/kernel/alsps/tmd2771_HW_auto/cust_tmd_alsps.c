#include <linux/types.h>
#include <mach/mt_pm_ldo.h>
#include <cust_alsps.h>

static struct alsps_hw cust_tmd_alsps_hw = {
    .i2c_num    = 2,
	.polling_mode_ps =0,
	.polling_mode_als =1,
    .power_id   = MT65XX_POWER_NONE,    /*LDO is not used*/
    .power_vol  = VOL_DEFAULT,          /*LDO is not used*/
    .i2c_addr   = {0x72, 0x48, 0x78, 0x00},
    .als_level  = {42,161,281,520,830,1140,1450},
    .als_value  = {10,255,320,640,1280,2600,10240},
    .ps_threshold = 900,
    .ps_threshold_high = 160,
    .ps_threshold_low = 120,
};

struct alsps_hw *get_tmd_cust_alsps_hw(void) 
{
        return &cust_tmd_alsps_hw;
    
}

int TMD2771_CMM_PPCOUNT_VALUE = 0;
/* modify the als_ps sensor parameter */
int TMD2771_G610T_CMM_PPCOUNT_VALUE = 0x02;
int TMD2771_G520_CMM_PPCOUNT_VALUE = 0x04;
int TMD2771_G700U_CMM_PPCOUNT_VALUE = 0x02;
int TMD2771_G610U_CMM_PPCOUNT_VALUE = 0x02;
int TMD2771_G730_CMM_PPCOUNT_VALUE = 0x06;
int TMD2771_H30_CMM_PPCOUNT_VALUE = 0x02;
int ZOOM_TIME = 5;
int TMD2771_CMM_CONTROL_VALUE = 0x20;
