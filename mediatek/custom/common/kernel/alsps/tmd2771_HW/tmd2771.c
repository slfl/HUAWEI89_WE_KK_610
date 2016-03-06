/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* drivers/hwmon/mt6516/amit/tmd2771.c - TMD2771 ALS/PS driver
 * 
 * Author: MingHsien Hsieh <minghsien.hsieh@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>

//#include <mach/mt_devs.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>

#define POWER_NONE_MACRO MT65XX_POWER_NONE
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#include <asm/io.h>
#include <cust_eint.h>
#include <cust_alsps.h>
#include "tmd2771.h"
#include <linux/i2c.h>
#include <linux/hardware_self_adapt.h>
#include <mtk_kpd.h>
/******************************************************************************
 * configuration
*******************************************************************************/
/*----------------------------------------------------------------------------*/

#define TMD2771_DEV_NAME     "TMD2771"
/*----------------------------------------------------------------------------*/
extern int TMD2771_G520_CMM_PPCOUNT_VALUE;
extern int TMD2771_CMM_PPCOUNT_VALUE;
/*shorten the bright screen time during a call */
static int tmd2771_debug_mask = 0;
static int ps_read_value= 0;
static int als_read_value = 0;
#define TMD27111_DBG(x...) do {\
    if (tmd2771_debug_mask) \
        printk( x);\
    } while (0)
module_param_named(tmd2771_debug, tmd2771_debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP);
#define APS_TAG                  "[ALS/PS] "
#define APS_FUN(f)               TMD27111_DBG(KERN_INFO APS_TAG"%s\n", __FUNCTION__)
#define APS_ERR(fmt, args...)    TMD27111_DBG(KERN_ERR  APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    TMD27111_DBG(KERN_INFO APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    TMD27111_DBG(KERN_INFO APS_TAG fmt, ##args)
#define ALS_TAG "[ALS] "
#define PS_TAG "[PS] "
#define ALS_LOG(fmt, args...) TMD27111_DBG(KERN_INFO ALS_TAG fmt, ## args)
#define PS_LOG(fmt, args...) TMD27111_DBG(KERN_INFO PS_TAG fmt, ## args)
#define ALS_ERR(fmt, args...) TMD27111_DBG(KERN_ERR ALS_TAG "%s %d : " fmt, __func__, __LINE__, ## args)
#define PS_ERR(fmt, args...) TMD27111_DBG(KERN_ERR PS_TAG "%s %d : " fmt, __func__, __LINE__, ## args)

/******************************************************************************
 * extern functions
*******************************************************************************/
/*for interrup work mode support --add by liaoxl.lenovo 12.08.2011*/

	extern void mt_eint_unmask(unsigned int line);
	extern void mt_eint_mask(unsigned int line);
	extern void mt_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
	extern void mt_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
	extern kal_uint32 mt_eint_set_sens(kal_uint8 eintno, kal_bool sens);
	extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
										 kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
										 kal_bool auto_umask);
	
/*----------------------------------------------------------------------------*/
static struct i2c_client *tmd2771_i2c_client = NULL;
/*----------------------------------------------------------------------------*/
static const struct i2c_device_id tmd2771_i2c_id[] = {{TMD2771_DEV_NAME,0},{}};
static struct i2c_board_info __initdata i2c_TMD2771={ I2C_BOARD_INFO("TMD2771", (0X72>>1))};
/*the adapter id & i2c address will be available in customization*/
//static unsigned short tmd2771_force[] = {0x02, 0X72, I2C_CLIENT_END, I2C_CLIENT_END};
//static const unsigned short *const tmd2771_forces[] = { tmd2771_force, NULL };
//static struct i2c_client_address_data tmd2771_addr_data = { .forces = tmd2771_forces,};
/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int tmd2771_i2c_remove(struct i2c_client *client);
static int tmd2771_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_suspend(struct i2c_client *client, pm_message_t msg);
static int tmd2771_i2c_resume(struct i2c_client *client);

static struct tmd2771_priv *g_tmd2771_ptr = NULL;

 struct PS_CALI_DATA_STRUCT
{
    int close;
    int far_away;
    int valid;
} ;

static struct PS_CALI_DATA_STRUCT ps_cali={{960,959,1},};

/*----------------------------------------------------------------------------*/
typedef enum {
    CMC_BIT_ALS    = 1,
    CMC_BIT_PS     = 2,
} CMC_BIT;
/*----------------------------------------------------------------------------*/
struct tmd2771_i2c_addr {    /*define a series of i2c slave address*/
    u8  write_addr;  
    u8  ps_thd;     /*PS INT threshold*/
};
/*----------------------------------------------------------------------------*/
struct tmd2771_priv {
    struct alsps_hw  *hw;
    struct i2c_client *client;
    struct work_struct  eint_work;

    /*i2c address group*/
    struct tmd2771_i2c_addr  addr;
    
    /*misc*/
    u16		    als_modulus;
    atomic_t    i2c_retry;
    atomic_t    als_suspend;
    atomic_t    als_debounce;   /*debounce time after enabling als*/
    atomic_t    als_deb_on;     /*indicates if the debounce is on*/
    atomic_t    als_deb_end;    /*the jiffies representing the end of debounce*/
    atomic_t    ps_mask;        /*mask ps: always return far away*/
    atomic_t    ps_debounce;    /*debounce time after enabling ps*/
    atomic_t    ps_deb_on;      /*indicates if the debounce is on*/
    atomic_t    ps_deb_end;     /*the jiffies representing the end of debounce*/
    atomic_t    ps_suspend;


    /*data*/
    u16         als;
    u16          ps;
    u8          _align;
    u16         als_level_num;
    u16         als_value_num;
    u32         als_level[C_CUST_ALS_LEVEL];
    u32         als_value[C_CUST_ALS_LEVEL];

    atomic_t    als_cmd_val;    /*the cmd value can't be read, stored in ram*/
    atomic_t    ps_cmd_val;     /*the cmd value can't be read, stored in ram*/
    atomic_t    ps_thd_val;     /*the cmd value can't be read, stored in ram*/
    ulong       enable;         /*enable mask*/
    ulong       pending_intr;   /*pending interrupt*/

    /*early suspend*/
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif     
};
/*----------------------------------------------------------------------------*/
static struct i2c_driver tmd2771_i2c_driver = {	
	.probe      = tmd2771_i2c_probe,
	.remove     = tmd2771_i2c_remove,
	.detect     = tmd2771_i2c_detect,
	.suspend    = tmd2771_i2c_suspend,
	.resume     = tmd2771_i2c_resume,
	.id_table   = tmd2771_i2c_id,
//	.address_data = &tmd2771_addr_data,
	.driver = {
//		.owner          = THIS_MODULE,
		.name           = TMD2771_DEV_NAME,
	},
};

static struct tmd2771_priv *tmd2771_obj = NULL;
static struct platform_driver tmd2771_alsps_driver;
static int min_proximity_value = 822;
static int pwin_value = 200;
static int pwave_value = 200;
static int aps_first_read = 1;
#define LSENSOR_MAX_LEVEL C_CUST_ALS_LEVEL
/*shorten the bright screen time during a call */
#define TMD2771_CMM_PER_VALUE 0x10
#define TMD2771_CMM_ATIME_VALUE 0xDB

/* Visible compensation : 8 times
   IR compensation : 3 times 
 */
static int scacle_factor_vasible = 9;
static int scacle_factor_ir = 3;
static int open_count_als = 0;    
struct mutex  mlock; /* lock for access tmd2711 */
#define TMD2771_CMM_CONTROL_VALUE 0x20
#define OPEN_COUNT_MAX  10    
static int tmd2771_check_and_clear_intr(struct i2c_client *client);
static uint8_t tmd2711_irq_flag = 0;
static int tmd2771_create_attr(struct device_driver *driver);
static int tmd2771_delete_attr(struct device_driver *driver);
/*****************************************************************************
 Prototype    : set_tmd2771_register
 Description  : set the register's value on the tmd2711 chip
 Input        : struct tmd2771_priv  *aps  
                u8 reg
                u16 value
                int flag
 Output       : None
 Return Value : static
 Calls        :
 Called By    :

  History        :
  1.Date         : 2012/3/5
    Author       : lkf61763
    Modification : Created function

*****************************************************************************/
static int  set_tmd2771_register(struct tmd2771_priv  *aps, u8 reg, u16 value, int flag)
{
	int ret;
    reg &= 0x1f;
    mutex_lock(&mlock);
	if(flag)
	{
        ret = i2c_smbus_write_word_data(aps->client, CMD_WORD | reg, value);
	}
	else
	{
        ret = i2c_smbus_write_byte_data(aps->client, CMD_BYTE | reg, (u8)value);
	}
    mutex_unlock(&mlock);
	return ret;
}

/*****************************************************************************
 Prototype    : get_tmd2771_register
 Description  : Read the value of the specified register in the chip
 Input        : struct tmd2771_priv  *aps  
                u8 reg
                int flag
 Output       : None
 Return Value : static
 Calls        :
 Called By    :

  History        :
  1.Date         : 2012/3/5
    Author       : lkf61763
    Modification : Created function

*****************************************************************************/
static int get_tmd2771_register(struct tmd2771_priv  *aps, u8 reg, int flag)
{
    int ret;
    reg &= 0x1f;
    mutex_lock(&mlock);
    if (flag)
    {
        ret = i2c_smbus_read_word_data(aps->client, CMD_WORD | reg);
    }
    else
    {
        ret = i2c_smbus_read_byte_data(aps->client, CMD_BYTE | reg);
    }
    mutex_unlock(&mlock);
    return ret;
}

/* Coefficients in open air:
 * GA:Glass (or Lens) Attenuation Factor
 * DF:Device Factor
 * alsGain: ALS Gain
 * aTime: ALS Timing
 * ALSIT = 2.72ms * (256 ẀC ATIME) = 2.72ms * (256-0xDB) =  100ms
 */

/*
	ATIME_ms = 2.72*(0xff-TMD2771_CMM_ATIME_VALUE+1)
	AGAINx = 120
	GA = 1;
	CPL = (ATIME_ms * AGAINx) / (GA  * 24)
	Lux1 = (C0DATA - 2 * C1DATA) / CPL
		   =  ((C0DATA - 2 * C1DATA) *(GA  * 24)) / (ATIME_ms * AGAINx)
		   =  (100*(C0DATA - 2 * C1DATA) *(GA  * 24)) / ((272*(0xff-TMD2771_CMM_ATIME_VALUE+1)) * AGAINx)
	Lux2 = (0.6 * C0DATA-C1DATA) / CPL
		   =  ((0.6 * C0DATA-C1DATA) *(GA  * 24)) / (ATIME_ms * AGAINx)
		   =  (100*(0.6 * C0DATA-C1DATA) *(GA  * 24)) / ((272*(0xff-TMD2771_CMM_ATIME_VALUE+1)) * AGAINx)
		   =  (60 * C0DATA-100*C1DATA) *(GA  * 24)) / ((272*(0xff-TMD2771_CMM_ATIME_VALUE+1)) * AGAINx)
	Lux = MAX(Lux1, Lux2, 0)
 */
#define TMD2771_CMM_ATIME_MS (2.72 * (0xff - TMD2771_CMM_ATIME_VALUE + 1))
#define TMD2771_CMM_AGAINx 120
#define TMD2771_CMM_GA 1

const int againx_table[] = {0x01, 0x08, 0x10, 0x78};
/*****************************************************************************
 Prototype    : luxcalculation
 Description  : Calculate the intensity of illumination of the environment
 Input        : struct tmd2771_priv  *aps
                int c0data
                int c1data
 Output       : None
 Return Value : static
 Calls        :
 Called By    :

  History        :
  1.Date         : 2012/3/5
    Author       : lkf61763
    Modification : Created function

*****************************************************************************/
static int luxcalculation(struct tmd2771_priv  *aps,int c0data, int c1data)
{
	int luxValue = 0;
	int control_reg = 0;
	int cp_temp;
	int lux_temp1;
	int lux_temp2;

	control_reg = get_tmd2771_register(aps, TMD2771_CMM_CONTROL, 0);
	control_reg &=0x03;

	cp_temp   = (againx_table[control_reg] * (272 * (0xff - TMD2771_CMM_ATIME_VALUE + 1))) / (TMD2771_CMM_GA * 24);
    lux_temp1 = (100 * (c0data - 2 * c1data) ) / cp_temp;
    lux_temp2 = ((60 * c0data - 100 * c1data) ) / cp_temp;
	
	if (lux_temp1 > lux_temp2)
	{
		luxValue = lux_temp1;
	}
	else
	{
		if (lux_temp2 < 0)
		{
			luxValue = 0;
		}
        else
        {
            luxValue = lux_temp2;
        }
	}
    /* delete */
	return luxValue;
}

/*****************************************************************************
 Prototype    : tmd2771_read_ps
 Description  : Read the value of  proximity from the chip
 Input        : struct tmd2771_priv  *aps  
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2012/3/8
    Author       : lkf61763
    Modification : Created function

*****************************************************************************/
int tmd2771_read_ps(struct tmd2771_priv  *aps)
{
	int ret;
	int pdata;
	int pthreshold_h = 0, pthreshold_l;

	/* read the proximity data  */
	pdata = get_tmd2771_register(aps, TMD2771_CMM_PDATA_L, 1);

	/* add the arithmetic of setting the proximity thresholds automatically */
	/*shorten the bright screen time during a call */
	printk("pdata:%d min_proximity_value:%d", pdata, min_proximity_value);

       ps_read_value = pdata;
	if (((pdata + pwave_value) < min_proximity_value)&& (pdata>0))
	{
		min_proximity_value = pdata + pwave_value;
		ps_cali.close = min_proximity_value + pwin_value;
		if(ps_cali.close >= 1023)
		{
			ps_cali.close = 1023;
			min_proximity_value = 1023-pwin_value;
		}
		ps_cali.far_away = min_proximity_value;
		ret  = set_tmd2771_register(aps, TMD2771_CMM_INT_LOW_THD_LOW, min_proximity_value, 1);
		ret |= set_tmd2771_register(aps, TMD2771_CMM_INT_HIGH_THD_LOW, (min_proximity_value + pwin_value), 1);
		if (ret)
		{
			PS_ERR(":set TMD2771_CMM_INT_LOW_THD_LOW register is error(%d)!", ret);
		}

		PS_LOG("%s:min_proximity_value=%d\n", __func__, min_proximity_value);
		return;
	}

	pthreshold_h = get_tmd2771_register(aps, TMD2771_CMM_INT_HIGH_THD_LOW, 1);
	pthreshold_l = get_tmd2771_register(aps, TMD2771_CMM_INT_LOW_THD_LOW, 1);

	/* add some logs */
	PS_LOG("%s:pdata=%d pthreshold_h=%d pthreshold_l=%d\n", __func__, pdata, pthreshold_h, pthreshold_l);

	/* if more than the value of  proximity high threshold we set*/
	if (pdata >= pthreshold_h)
	{
		PS_LOG("set PILTL:%d\n", min_proximity_value);
		ret = set_tmd2771_register(aps, TMD2771_CMM_INT_LOW_THD_LOW, min_proximity_value, 1);
		ret |= set_tmd2771_register(aps, TMD2771_CMM_INT_HIGH_THD_LOW,1023, 1);
		if (ret)
		{
			PS_ERR(":set TMD2771_CMM_INT_LOW_THD_LOW register is error(%d)!", ret);
		}
		ps_cali.far_away = min_proximity_value;
		ps_cali.close = 1023;
		aps->ps = 0;
	}
	/* if less than the value of  proximity low threshold we set*/
	/* the condition of pdata==pthreshold_l is valid */
	else if (pdata <= pthreshold_l)
	{
		PS_LOG("set PILTL:0\n");
		ret = set_tmd2771_register(aps, TMD2771_CMM_INT_LOW_THD_LOW, min_proximity_value, 1);
		ret |= set_tmd2771_register(aps, TMD2771_CMM_INT_HIGH_THD_LOW,min_proximity_value + pwin_value, 1);
		ps_cali.far_away =min_proximity_value;
		ps_cali.close = min_proximity_value + pwin_value;
		if (ret)
		{
			PS_ERR(":set TMD2771_CMM_INT_LOW_THD_LOWs register is error(%d)!", ret);
		}
		else
		{
			aps->ps = 1;
		}
	}
	/*shorten the bright screen time during a call */
	printk("proximity send:%d\n", aps->ps);
}

/*****************************************************************************
 Prototype    : tmd2771_read_als
 Description  : Read ALS value of  proximity from the chip
 Input        : struct tmd2771_priv *aps
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2012/3/8
    Author       : lkf61763
    Modification : Created function

*****************************************************************************/
int tmd2771_read_als(struct tmd2771_priv  *aps)
{
	int cdata  = 0;/* ch0 data  */
	int irdata = 0;/* ch1 data  */
	int ret;
	int cdata_high = 0, cdata_low = 0;
	uint8_t als_level = 0;
	int lux;
	int i;

	/* read the CH0 data and CH1 data  */
	cdata  = get_tmd2771_register(aps, TMD2771_CMM_C0DATA_L, 1);
	irdata = get_tmd2771_register(aps, TMD2771_CMM_C1DATA_L, 1);
	if ((cdata <= 0) && (irdata <= 0))  //the tmd2771 chip is not ready for read data
	{
		printk("the tmd2771 chip is not ready for read data!!\n");
		return;
	}
	/* set ALS high threshold = ch0(cdata) + 20%,low threshold = ch0(cdata) - 20% */
	cdata_high = (cdata * 600) / 500;
	cdata_low = (cdata * 400) / 500;
    /* delete */

	ret  = set_tmd2771_register(aps, TMD2771_CMM_AILTL, cdata_low, 1);
	ret |= set_tmd2771_register(aps, TMD2771_CMM_AIHTL, cdata_high, 1);
	if (ret)
	{
		ALS_ERR( ":set TMD2771_CMM_AILTL register is error(%d)!", ret);
	}

    /* Visible compensation	*/
    cdata = cdata*scacle_factor_vasible;
    /* IR compensation */
    irdata = irdata*scacle_factor_ir;
	/* convert the raw pdata and irdata to the value in units of lux */
	lux = luxcalculation(aps,cdata, irdata);

	/* lux=0 is valid */
	ALS_LOG("%s:cdata=%d irdata=%d lux=%d aps_first_read:%d\n", __func__, cdata, irdata, lux, aps_first_read);
	if (lux >= 0)
	{
		if (aps_first_read)
		{
			aps_first_read = 0;
			aps->als = 0;
		}
		else
		{
			als_level = LSENSOR_MAX_LEVEL - 1;
			for (i = 0; i < ARRAY_SIZE(aps->hw->als_level); i++)
			{
                if (lux < aps->hw->als_level[i])
				{
					als_level = i;
					break;
				}
			}

			aps->als = aps->hw->als_value[als_level];
		}
             als_read_value = aps->als;
	}
	/* if lux<0,we need to change the gain which we can set register 0x0f */
	else
	{
		ALS_LOG("Need to change gain %2d \n", lux);
             als_read_value = 0;
	}
    /* delete */
}


/*----------------------------------------------------------------------------*/
int tmd2771_get_addr(struct alsps_hw *hw, struct tmd2771_i2c_addr *addr)
{
	if(!hw || !addr)
	{
		return -EFAULT;
	}
	addr->write_addr= hw->i2c_addr[0];
	return 0;
}
/*----------------------------------------------------------------------------*/
static void tmd2771_power(struct alsps_hw *hw, unsigned int on) 
{
	static unsigned int power_on = 0;

	//APS_LOG("power %s\n", on ? "on" : "off");

	if(hw->power_id != POWER_NONE_MACRO)
	{
		if(power_on == on)
		{
			APS_LOG("ignore power control: %d\n", on);
		}
		else if(on)
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "TMD2771")) 
			{
				APS_ERR("power on fails!!\n");
			}
		}
		else
		{
			if(!hwPowerDown(hw->power_id, "TMD2771")) 
			{
				APS_ERR("power off fail!!\n");   
			}
		}
	}
	power_on = on;
}
/*----------------------------------------------------------------------------*/
static long tmd2771_enable_als(struct i2c_client *client, int enable)
{
	struct tmd2771_priv *obj = i2c_get_clientdata(client);
	u8 databuf[2];
	long res = 0;
	u8 buffer[1];
	u8 reg_value[1];
	uint32_t testbit_PS;

	if (client == NULL)
	{
		ALS_LOG("CLIENT CANN'T EQUL NULL\n");
		return -1;
	}
	/*enable_als function modified for fixing reading register error problem */
	testbit_PS = test_bit(CMC_BIT_PS, &obj->enable) ? (1) : (0);
	if (enable)
	{
		if (testbit_PS)
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x2F;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}
		else
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x0B;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}

		atomic_set(&obj->als_deb_on, 1);
		atomic_set(&obj->als_deb_end, jiffies + atomic_read(&obj->als_debounce) / (1000 / HZ));
		ALS_LOG("tmd2771 power on\n");
	}
	else
	{
		if (testbit_PS)
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x2D;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}
		else
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x09;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}

		/*Lenovo-sw chenlj2 add 2011-06-03,modify ps_deb_on to als_deb_on */
		atomic_set(&obj->als_deb_on, 0);
		ALS_LOG("tmd2771 power off\n");
	}
	return 0;

EXIT_ERR:
	ALS_ERR("tmd2771_enable_als fail\n");
	return res;
}

/*----------------------------------------------------------------------------*/
static long tmd2771_enable_ps(struct i2c_client *client, int enable)
{
	struct tmd2771_priv *obj = i2c_get_clientdata(client);
	u8 databuf[2];
	long res = 0;
	u8 buffer[1];
	u8 reg_value[1];
	uint32_t testbit_ALS;

	if (client == NULL)
	{
		APS_DBG("CLIENT CANN'T EQUL NULL\n");
		return -1;
	}

	/*enable_ps function modified for fixing reading register error problem */
	testbit_ALS = test_bit(CMC_BIT_ALS, &obj->enable) ? (1) : (0);
	if (enable)
	{
		if (testbit_ALS)
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x0F;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}
		else
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x0D;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}

		atomic_set(&obj->ps_deb_on, 1);
		atomic_set(&obj->ps_deb_end, jiffies + atomic_read(&obj->ps_debounce) / (1000 / HZ));
		PS_LOG("tmd2771 power on\n");

		/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
		if (0 == obj->hw->polling_mode_ps)
		{
			res = set_tmd2771_register(obj, TMD2771_CMM_INT_LOW_THD_LOW, ps_cali.far_away, 1);

			if (res)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}

			res = set_tmd2771_register(obj, TMD2771_CMM_INT_HIGH_THD_LOW, ps_cali.close, 1);
			if (res)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}

			databuf[0] = TMD2771_CMM_Persistence;
			databuf[1] = TMD2771_CMM_PER_VALUE;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}

			if (testbit_ALS)
			{
				databuf[0] = TMD2771_CMM_ENABLE;
				databuf[1] = 0x2F;
				res = i2c_master_send(client, databuf, 0x2);
				if (res <= 0)
				{
					goto EXIT_ERR;
				}

			}
			else
			{
				databuf[0] = TMD2771_CMM_ENABLE;
				databuf[1] = 0x2D;
				res = i2c_master_send(client, databuf, 0x2);
				if (res <= 0)
				{
					goto EXIT_ERR;
				}
			}

			mt_eint_unmask(CUST_EINT_ALS_NUM);
		}
	}
	else
	{
		/*yucong MTK: enable_ps function modified for fixing reading register error problem 2012.2.16*/
		if (testbit_ALS)
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x2B;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}
		else
		{
			databuf[0] = TMD2771_CMM_ENABLE;
			databuf[1] = 0x29;
			res = i2c_master_send(client, databuf, 0x2);
			if (res <= 0)
			{
				goto EXIT_ERR;
			}
		}

		atomic_set(&obj->ps_deb_on, 0);
		PS_LOG("tmd2771 power off\n");

		/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
		if (0 == obj->hw->polling_mode_ps)
		{
			cancel_work_sync(&obj->eint_work);
			mt_eint_mask(CUST_EINT_ALS_NUM);
		}
	}
	PS_LOG("tmd2771_enable_ps,ps_cali.far_away=%d,ps_cali.close=%d\n",ps_cali.far_away,ps_cali.close);
	return 0;

EXIT_ERR:
	PS_ERR("tmd2771_enable_ps fail\n");
	return res;
}
/*****************************************************************************
 Prototype    : aps_tmd2771_work_func
 Description  : Read sensor's ADC value processing its value and reported
                it to HWMSEN
 Input        : struct work_struct *work  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/3/5
    Author       : lkf61763
    Modification : Created function

*****************************************************************************/
static void aps_tmd2771_work_func(struct work_struct *work)
{
	int ret;

	struct tmd2771_priv *aps = (struct tmd2771_priv *)container_of(work, struct tmd2771_priv, eint_work);
	hwm_sensor_data sensor_data;

 	mt_eint_mask(CUST_EINT_ALS_NUM);

	if ((ret = tmd2771_check_and_clear_intr(aps->client)))
	{
		APS_ERR("aps_tmd2771_work_func check intrs: %d\n", ret);
		tmd2711_irq_flag = 0;

		atomic_set(&aps->als_suspend, 0);
 		if(test_bit(CMC_BIT_ALS, &aps->enable))
		{
			if (ret = tmd2771_enable_als(aps->client, 1))
			{
				ALS_ERR("enable als fail: %d\n", ret);
			}
		}

		atomic_set(&aps->ps_suspend, 0);
		if(test_bit(CMC_BIT_PS, &aps->enable))
		{
			if (ret = tmd2771_enable_ps(aps->client, 1))
			{
				ALS_ERR("enable als fail: %d\n", ret);
			}
		}

		mt_eint_unmask(CUST_EINT_ALS_NUM);

		return;
	}

	APS_LOG("aps_tmd2771_work_func enable:%d status:%x\n", aps->enable, tmd2711_irq_flag);

	/* proximity flag is open and the interrupt belongs to proximity */
	if ((test_bit(CMC_BIT_PS, &aps->enable)) && (tmd2711_irq_flag & TMD2771_STATUS_PROXIMITY_BIT))
	{
		tmd2711_irq_flag &= ~TMD2771_STATUS_PROXIMITY_BIT;

		if(0 == aps->hw->polling_mode_ps)
		{
			tmd2771_read_ps(aps);

			sensor_data.values[0] = aps->ps;
			sensor_data.value_divide = 1;
			sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;

			//let up layer to know
			if ((ret = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
			{
				PS_ERR("call hwmsen_get_interrupt_data fail = %d\n", ret);
			}
		}
	}

	/* ALS flag is open and the interrupt belongs to ALS */
	if ((test_bit(CMC_BIT_ALS, &aps->enable)) && (tmd2711_irq_flag & TMD2771_STATUS_ALS_BIT))
	{
		tmd2711_irq_flag &= ~TMD2771_STATUS_ALS_BIT;

		if(0 == aps->hw->polling_mode_als)
		{
			tmd2771_read_als(aps);

			sensor_data.values[0] = aps->ps;
			sensor_data.value_divide = 1;
			sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;

			//let up layer to know
			if ((ret = hwmsen_get_interrupt_data(ID_LIGHT, &sensor_data)))
			{
				PS_ERR("call hwmsen_get_interrupt_data fail = %d\n", ret);
			}
		}
	}

	mt_eint_unmask(CUST_EINT_ALS_NUM);
}
/*delete*/

/*----------------------------------------------------------------------------*/
/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
static int tmd2771_check_and_clear_intr(struct i2c_client *client) 
{
	struct tmd2771_priv *obj = i2c_get_clientdata(client);
	int res,intp,intl;
	u8 buffer[2];

	//if (mt_get_gpio_in(GPIO_ALS_EINT_PIN) == 1) /*skip if no interrupt*/  
	//    return 0;

	buffer[0] = TMD2771_CMM_STATUS;
	res = i2c_master_send(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	res = i2c_master_recv(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	//APS_ERR("tmd2771_check_and_clear_intr status=0x%x\n", buffer[0]);
	res = 1;
	intp = 0;
	intl = 0;
	if(0 != (buffer[0] & 0x20))
	{
		res = 0;
		intp = 1;
		tmd2711_irq_flag |= TMD2771_STATUS_PROXIMITY_BIT;
	}
	if(0 != (buffer[0] & 0x10))
	{
		res = 0;
		intl = 1;		
		tmd2711_irq_flag |= TMD2771_STATUS_ALS_BIT;
	}

	if(0 == res)
	{
		if((1 == intp) && (0 == intl))
		{
			buffer[0] = (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x05);
		}
		else if((0 == intp) && (1 == intl))
		{
			buffer[0] = (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x06);
		}
		else
		{
			buffer[0] = (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x07);
		}
		res = i2c_master_send(client, buffer, 0x1);
		if(res <= 0)
		{
			goto EXIT_ERR;
		}
		else
		{
			res = 0;
		}
	}

	return res;

EXIT_ERR:
	APS_ERR("tmd2771_check_and_clear_intr fail\n");
	return 1;
}
/*----------------------------------------------------------------------------*/
void tmd2771_eint_func(void)
{
	struct tmd2771_priv *obj = g_tmd2771_ptr;
	if(!obj)
	{
		return;
	}
	
	schedule_work(&obj->eint_work);
}

/*----------------------------------------------------------------------------*/
/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
int tmd2771_setup_eint(struct i2c_client *client)
{
	struct tmd2771_priv *obj = i2c_get_clientdata(client);        

	g_tmd2771_ptr = obj;
	
	mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
	mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, TRUE);
	mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);

	mt_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	mt_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	mt_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, tmd2771_eint_func, 0);

	mt_eint_mask(CUST_EINT_ALS_NUM);
    return 0;
}


/* delete */

static int tmd2771_init_client(struct i2c_client *client)
{
	struct tmd2771_priv *obj = i2c_get_clientdata(client);
	u8 databuf[2];    
	int res = 0;
   
	databuf[0] = TMD2771_CMM_ENABLE;    
	databuf[1] = 0x01;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}
	
	databuf[0] = TMD2771_CMM_ATIME;    
    databuf[1] = TMD2771_CMM_ATIME_VALUE;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}

	databuf[0] = TMD2771_CMM_PTIME;    
	databuf[1] = 0xFF;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}

	databuf[0] = TMD2771_CMM_WTIME;    
	databuf[1] = 0xEE;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}
	/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
	if(0 == obj->hw->polling_mode_ps)
	{
		if(1 == ps_cali.valid)
		{
			databuf[0] = TMD2771_CMM_INT_LOW_THD_LOW;	
			databuf[1] = (u8)(ps_cali.far_away & 0x00FF);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
			databuf[0] = TMD2771_CMM_INT_LOW_THD_HIGH;	
			databuf[1] = (u8)((ps_cali.far_away & 0xFF00) >> 8);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
			databuf[0] = TMD2771_CMM_INT_HIGH_THD_LOW;	
			databuf[1] = (u8)(ps_cali.close & 0x00FF);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
			databuf[0] = TMD2771_CMM_INT_HIGH_THD_HIGH;
			databuf[1] = (u8)((ps_cali.close & 0xFF00) >> 8);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
		}
		else
		{
			databuf[0] = TMD2771_CMM_INT_LOW_THD_LOW;	
			databuf[1] = (u8)(480 & 0x00FF);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
			databuf[0] = TMD2771_CMM_INT_LOW_THD_HIGH;	
			databuf[1] = (u8)((480 & 0xFF00) >> 8);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
			databuf[0] = TMD2771_CMM_INT_HIGH_THD_LOW;	
			databuf[1] = (u8)(700 & 0x00FF);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}
			databuf[0] = TMD2771_CMM_INT_HIGH_THD_HIGH;	
			databuf[1] = (u8)((700 & 0xFF00) >> 8);
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				goto EXIT_ERR;
				return TMD2771_ERR_I2C;
			}

		}

		databuf[0] = TMD2771_CMM_Persistence;
        databuf[1] = TMD2771_CMM_PER_VALUE;
		res = i2c_master_send(client, databuf, 0x2);
		if(res <= 0)
		{
			goto EXIT_ERR;
			return TMD2771_ERR_I2C;
		}
		databuf[0] = TMD2771_CMM_ENABLE;	
		databuf[1] = 0x01 | 0x20;
		res = i2c_master_send(client, databuf, 0x2);
		if(res <= 0)
		{
			goto EXIT_ERR;
			return TMD2771_ERR_I2C;
		}

	}

	databuf[0] = TMD2771_CMM_CONFIG;    
	databuf[1] = 0x00;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}

       /*Lenovo-sw chenlj2 add 2011-06-03,modified pulse 2  to 4 */
	databuf[0] = TMD2771_CMM_PPCOUNT;    
	databuf[1] = TMD2771_CMM_PPCOUNT_VALUE;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}

        /*Lenovo-sw chenlj2 add 2011-06-03,modified gain 16  to 1 */
	databuf[0] = TMD2771_CMM_CONTROL;
	databuf[1] = TMD2771_CMM_CONTROL_VALUE;
	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		goto EXIT_ERR;
		return TMD2771_ERR_I2C;
	}
	/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
	if(res = tmd2771_setup_eint(client))
	{
		APS_ERR("setup eint: %d\n", res);
		return res;
	}
	if(res = tmd2771_check_and_clear_intr(client))
	{
		APS_ERR("check/clear intr: %d\n", res);
		//    return res;
	}
	
	return TMD2771_SUCCESS;

EXIT_ERR:
	APS_ERR("init dev: %d\n", res);
	return res;
}

/******************************************************************************
* Function Configuration
******************************************************************************/
static int tmd2771_open(struct inode *inode, struct file *file)
{
	file->private_data = tmd2771_i2c_client;
	aps_first_read = 1;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int tmd2771_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*delete*/



/*----------------------------------------------------------------------------*/
static long tmd2771_unlocked_ioctl(struct file *file, unsigned int cmd,
       unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct tmd2771_priv *obj = i2c_get_clientdata(client);  
	long err = 0;
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;
	struct PS_CALI_DATA_STRUCT ps_cali_temp;

	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if(err = tmd2771_enable_ps(obj->client, 1))
				{
					APS_ERR("enable ps fail: %d\n", err); 
					goto err_out;
				}
				
				set_bit(CMC_BIT_PS, &obj->enable);
			}
			else
			{
				if(err = tmd2771_enable_ps(obj->client, 0))
				{
					APS_ERR("disable ps fail: %d\n", err); 
					goto err_out;
				}
				
				clear_bit(CMC_BIT_PS, &obj->enable);
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(CMC_BIT_PS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_DATA:    

		if (copy_to_user(ptr, &obj->ps, sizeof(obj->ps)))
		{
			err = -EFAULT;
			goto err_out;
		}
			break;

		case ALSPS_GET_PS_RAW_DATA:    

		if (copy_to_user(ptr, &obj->ps, sizeof(obj->ps)))
		{
			err = -EFAULT;
			goto err_out;
		}
			break;              

		case ALSPS_SET_ALS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if(err = tmd2771_enable_als(obj->client, 1))
				{
					APS_ERR("enable als fail: %d\n", err); 
					goto err_out;
				}
				set_bit(CMC_BIT_ALS, &obj->enable);
			}
			else
			{
				if(err = tmd2771_enable_als(obj->client, 0))
				{
					APS_ERR("disable als fail: %d\n", err); 
					goto err_out;
				}
				clear_bit(CMC_BIT_ALS, &obj->enable);
			}
			break;

		case ALSPS_GET_ALS_MODE:
			enable = test_bit(CMC_BIT_ALS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_DATA: 
		if (copy_to_user(ptr, &obj->als, sizeof(obj->als)))
		{
			err = -EFAULT;
			goto err_out;
		}
			break;

		case ALSPS_GET_ALS_RAW_DATA:    
		if (copy_to_user(ptr, &obj->als, sizeof(obj->als)))
		{
			err = -EFAULT;
			goto err_out;
		}
			break;
		
		default:
			APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

	err_out:
	return err;    
}
/*----------------------------------------------------------------------------*/
static struct file_operations tmd2771_fops = {
	.owner = THIS_MODULE,
	.open = tmd2771_open,
	.release = tmd2771_release,
	.unlocked_ioctl = tmd2771_unlocked_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice tmd2771_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &tmd2771_fops,
};
/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_suspend(struct i2c_client *client, pm_message_t msg) 
{
	//struct tmd2771_priv *obj = i2c_get_clientdata(client);    
	//int err;
	APS_FUN();    

	return 0;
}
/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_resume(struct i2c_client *client)
{
	//struct tmd2771_priv *obj = i2c_get_clientdata(client);        
	//int err;
	APS_FUN();

	return 0;
}
/*----------------------------------------------------------------------------*/
static void tmd2771_early_suspend(struct early_suspend *h) 
{   /*early_suspend is only applied for ALS*/
	struct tmd2771_priv *obj = container_of(h, struct tmd2771_priv, early_drv);   
	int err;
	int reg_value = 0;
	if(test_bit(CMC_BIT_PS, &obj->enable))
	{
		return;
	}
	mt_eint_mask(CUST_EINT_ALS_NUM);
	APS_FUN();    

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 1);
	if (err = tmd2771_enable_als(obj->client, 0))
	{
		APS_ERR("disable als fail: %d\n", err);
	}

	atomic_set(&obj->ps_suspend, 1);
	if (err = tmd2771_enable_ps(obj->client, 0))
	{
		PS_ERR("enable als fail: %d\n", err);
	}

	reg_value = get_tmd2771_register(obj,TMD2771_CMM_ENABLE,0);
	reg_value &=0xfe;
	set_tmd2771_register(obj,TMD2771_CMM_ENABLE,reg_value,0);
	return;	

}
/*----------------------------------------------------------------------------*/
static void tmd2771_late_resume(struct early_suspend *h)
{   /*early_suspend is only applied for ALS*/
	struct tmd2771_priv *obj = container_of(h, struct tmd2771_priv, early_drv);         
	int err;
	int reg_value = 0;
	APS_FUN();

	reg_value = get_tmd2771_register(obj,TMD2771_CMM_ENABLE,0);
	reg_value |=0x01;
	set_tmd2771_register(obj,TMD2771_CMM_ENABLE,reg_value,0);

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 0);
	if(test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if (err = tmd2771_enable_als(obj->client, 1))
		{
			ALS_ERR("enable als fail: %d\n", err);
		}
	}


	atomic_set(&obj->ps_suspend, 0);
	if(test_bit(CMC_BIT_PS, &obj->enable))
	{
		if (err = tmd2771_enable_ps(obj->client, 1))
		{
			ALS_ERR("enable als fail: %d\n", err);
		}
	}
	if ((0 == obj->hw->polling_mode_ps) || (0 == obj->hw->polling_mode_als))
	{
	 	mt_eint_unmask(CUST_EINT_ALS_NUM);
	}

}

int tmd2771_ps_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct tmd2771_priv *obj = (struct tmd2771_priv *)self;
	
	PS_LOG("tmd2771_ps_operate cmd :%d\n",command);
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{				
				value = *(int *)buff_in;

				if(value)
				{
					if(err = tmd2771_enable_ps(obj->client, 1))
					{
						APS_ERR("enable ps fail: %d\n", err); 
						return -1;
					}
					set_bit(CMC_BIT_PS, &obj->enable);
					ALS_LOG("tmd2771_ps_operate SENSOR_ENABLE,disable ");
					#if 0	
					if(err = tmd2771_enable_als(obj->client, 1))
					{
						APS_ERR("enable als fail: %d\n", err); 
						return -1;
					}
					set_bit(CMC_BIT_ALS, &obj->enable);
					#endif
				}
				else
				{
					if(err = tmd2771_enable_ps(obj->client, 0))
					{
						APS_ERR("disable ps fail: %d\n", err); 
						return -1;
					}
					clear_bit(CMC_BIT_PS, &obj->enable);
					ALS_LOG("tmd2771_ps_operate SENSOR_ENABLE,disable ");
					#if 0
					if(err = tmd2771_enable_als(obj->client, 0))
					{
						APS_ERR("disable als fail: %d\n", err); 
						return -1;
					}
					clear_bit(CMC_BIT_ALS, &obj->enable);
					#endif
				}
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				if(0 != obj->hw->polling_mode_ps)
				{
					tmd2771_read_ps(obj);
				}

				sensor_data->values[0] = obj->ps;
				sensor_data->value_divide = 1;
				sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
			}
			break;
		default:
			APS_ERR("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}

static int temp_als = 0;

int tmd2771_als_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct tmd2771_priv *obj = (struct tmd2771_priv *)self;
	ALS_LOG("tmd2771_als_operate cmd :%d\n",command);

	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;				
				if(value)
				{
					ALS_LOG("tmd2771_als_operate SENSOR_ENABLE,enable ");
					
					if(err = tmd2771_enable_als(obj->client, 1))
					{
						APS_ERR("enable als fail: %d\n", err); 
						return -1;
					}
					set_bit(CMC_BIT_ALS, &obj->enable);
					
				}
				else
				{
					ALS_LOG("tmd2771_als_operate SENSOR_ENABLE,disable ");
					if(err = tmd2771_enable_als(obj->client, 0))
					{
						APS_ERR("disable als fail: %d\n", err); 
						return -1;
					}
					clear_bit(CMC_BIT_ALS, &obj->enable);
				}
				
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				if(0 != obj->hw->polling_mode_als)
				{
					tmd2771_read_als(obj);
				}
				
				sensor_data = (hwm_sensor_data *)buff_out;
				sensor_data->values[0] = obj->als;
				sensor_data->value_divide = 1;
				sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
			}
			break;
		default:
			APS_ERR("light sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}


/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) 
{    
	strcpy(info->type, TMD2771_DEV_NAME);
	return 0;
}

/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct tmd2771_priv *obj;
	struct hwmsen_object obj_ps, obj_als;
	int err = 0;

	    min_proximity_value = TMD2771_G610U_MIN_PRO_VALUE;
	    pwin_value = TMD2771_G610U_PWINDOWS_VALUE;
	    pwave_value = TMD2771_G610U_PWAVE_VALUE;
	    ps_cali.valid = 1;
	    ps_cali.close = 960;
	    ps_cali.far_away = 960 -TMD2771_G610U_PWINDOWS_VALUE ;
	    scacle_factor_vasible = 8;
	    scacle_factor_ir = 3;
	    TMD2771_CMM_PPCOUNT_VALUE = TMD2771_G610U_CMM_PPCOUNT_VALUE;

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	tmd2771_obj = obj;

	obj->hw = get_cust_alsps_hw();
	tmd2771_get_addr(obj->hw, &obj->addr);
    /* init mlock before interupting*/
    mutex_init(&mlock);
	/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
	INIT_WORK(&obj->eint_work, aps_tmd2771_work_func);
	obj->client = client;
	i2c_set_clientdata(client, obj);	
	atomic_set(&obj->als_debounce, 300);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 300);
	atomic_set(&obj->ps_deb_on, 0);
	atomic_set(&obj->ps_deb_end, 0);
	atomic_set(&obj->ps_mask, 0);
	atomic_set(&obj->als_suspend, 0);
	atomic_set(&obj->als_cmd_val, 0xDF);
	atomic_set(&obj->ps_cmd_val,  0xC1);
	atomic_set(&obj->ps_thd_val,  obj->hw->ps_threshold);
	/* Ensure LCD backlight is on in the begining of the first call*/
	obj->ps = 1;
	obj->enable = 0;
	obj->pending_intr = 0;
	obj->als_level_num = sizeof(obj->hw->als_level)/sizeof(obj->hw->als_level[0]);
	obj->als_value_num = sizeof(obj->hw->als_value)/sizeof(obj->hw->als_value[0]);  
	/*Lenovo-sw chenlj2 add 2011-06-03,modified gain 16 to 1/5 accoring to actual thing */
	obj->als_modulus = (400*100*ZOOM_TIME)/(1*150);//(1/Gain)*(400/Tine), this value is fix after init ATIME and CONTROL register value
										//(400)/16*2.72 here is amplify *100 //16
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	memcpy(obj->als_level, obj->hw->als_level, sizeof(obj->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	memcpy(obj->als_value, obj->hw->als_value, sizeof(obj->als_value));
	atomic_set(&obj->i2c_retry, 3);
	set_bit(CMC_BIT_ALS, &obj->enable);
	set_bit(CMC_BIT_PS, &obj->enable);

	
	tmd2771_i2c_client = client;

	
	if(err = tmd2771_init_client(client))
	{
		goto exit_init_failed;
	}
	APS_LOG("tmd2771_init_client() OK!\n");

	if(err = misc_register(&tmd2771_device))
	{
		APS_ERR("tmd2771_device register failed\n");
		goto exit_misc_device_register_failed;
	}
	if(err = tmd2771_create_attr(&tmd2771_alsps_driver.driver))
	{
		APS_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
	obj_ps.self = tmd2771_obj;
	/*for interrup work mode support -- by liaoxl.lenovo 12.08.2011*/
	if(1 == obj->hw->polling_mode_ps)
	{
		obj_ps.polling = 1;
	}
	else
	{
		obj_ps.polling = 0;
	}

    //mutex_init(&mlock);
	obj_ps.sensor_operate = tmd2771_ps_operate;
	if(err = hwmsen_attach(ID_PROXIMITY, &obj_ps))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}
	
	obj_als.self = tmd2771_obj;
	obj_als.polling = 1;
	obj_als.sensor_operate = tmd2771_als_operate;
	if(err = hwmsen_attach(ID_LIGHT, &obj_als))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}


#if defined(CONFIG_HAS_EARLYSUSPEND)
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	obj->early_drv.suspend  = tmd2771_early_suspend,
	obj->early_drv.resume   = tmd2771_late_resume,    
	register_early_suspend(&obj->early_drv);
#endif

	APS_LOG("%s: OK\n", __func__);
	return 0;

	exit_create_attr_failed:
	misc_deregister(&tmd2771_device);
	exit_misc_device_register_failed:
	exit_init_failed:
	//i2c_detach_client(client);
	exit_kfree:
	kfree(obj);
	exit:
	tmd2771_i2c_client = NULL;           
//	MT6516_EINTIRQMask(CUST_EINT_ALS_NUM);  /*mask interrupt if fail*/
	APS_ERR("%s: err = %d\n", __func__, err);
	return err;
}
/*----------------------------------------------------------------------------*/
static int tmd2771_i2c_remove(struct i2c_client *client)
{
	int err;
      if(err = tmd2771_delete_attr(&tmd2771_i2c_driver.driver))
	{
		APS_ERR("tmd2771_delete_attr fail: %d\n", err);
	}
      if(err = misc_deregister(&tmd2771_device))
	{
		APS_ERR("misc_deregister fail: %d\n", err);    
	}
	
	tmd2771_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));

	return 0;
}
/*----------------------------------------------------------------------------*/
static int tmd2771_probe(struct platform_device *pdev) 
{
	struct alsps_hw *hw = get_cust_alsps_hw();

	tmd2771_power(hw, 1);    
	//tmd2771_force[0] = hw->i2c_num;
	//tmd2771_force[1] = hw->i2c_addr[0];
	//APS_DBG("I2C = %d, addr =0x%x\n",tmd2771_force[0],tmd2771_force[1]);
	if(i2c_add_driver(&tmd2771_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 
	return 0;
}
/*----------------------------------------------------------------------------*/
static int tmd2771_remove(struct platform_device *pdev)
{
	struct alsps_hw *hw = get_cust_alsps_hw();
	APS_FUN();    
	tmd2771_power(hw, 0);    
	i2c_del_driver(&tmd2771_i2c_driver);
	return 0;
}
/*----------------------------------------------------------------------------*/
static struct platform_driver tmd2771_alsps_driver = {
	.probe      = tmd2771_probe,
	.remove     = tmd2771_remove,    
	.driver     = {
		.name  = "als_ps",
//		.owner = THIS_MODULE,
	}
};
static ssize_t show_reg_value(struct device_driver *ddri, char *buf)
{
    int value1 = 0;
    int value2 =0;
    value1 = get_tmd2771_register(tmd2771_obj, TMD2771_CMM_INT_LOW_THD_LOW, 1);
    value2 = get_tmd2771_register(tmd2771_obj, TMD2771_CMM_INT_HIGH_THD_LOW, 1);
    return sprintf(buf,"PS:\n TMD2771_CMM_INT_LOW_THD_LOW value1=:%d, TMD2771_CMM_INT_HIGH_THD_LOW value2=:%d \n",value1,value2);
}
static ssize_t show_ps_value(struct device_driver *ddri, char *buf)
{
    return sprintf(buf,"%d\n",ps_read_value);
}
static ssize_t show_als_value(struct device_driver *ddri, char *buf)
{
    return sprintf(buf,"%d\n",als_read_value);
}

static DRIVER_ATTR(ps_value,  S_IRUGO, show_ps_value, NULL);
static DRIVER_ATTR(als_value,  S_IRUGO, show_als_value, NULL);

static DRIVER_ATTR(reg_value,  S_IRUGO, show_reg_value, NULL);

/*----------------------------------------------------------------------------*/
static struct driver_attribute *tmd2771_attr_list[] = {
	&driver_attr_reg_value,
       &driver_attr_ps_value,
       &driver_attr_als_value
	
};
/*----------------------------------------------------------------------------*/
static int tmd2771_create_attr(struct device_driver *driver)
{
	int idx, err = 0;
	int num = (int)(sizeof(tmd2771_attr_list)/sizeof(tmd2771_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if(err = driver_create_file(driver, tmd2771_attr_list[idx]))
		{
			printk(KERN_ERR "driver_create_file (%s) = %d\n", tmd2771_attr_list[idx]->attr.name, err);
			break;
		}
	}
	return err;
}
/*----------------------------------------------------------------------------*/
static int tmd2771_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(tmd2771_attr_list)/sizeof(tmd2771_attr_list[0]));

	if(driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		driver_remove_file(driver, tmd2771_attr_list[idx]);
	}

	return err;
}
/*----------------------------------------------------------------------------*/
static int __init tmd2771_init(void)
{
	APS_FUN();
	i2c_register_board_info(3, &i2c_TMD2771, 1);
	if(platform_driver_register(&tmd2771_alsps_driver))
	{
		APS_ERR("failed to register driver");
		return -ENODEV;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit tmd2771_exit(void)
{
	APS_FUN();
	platform_driver_unregister(&tmd2771_alsps_driver);
}
/*----------------------------------------------------------------------------*/
module_init(tmd2771_init);
module_exit(tmd2771_exit);
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("Dexiang Liu");
MODULE_DESCRIPTION("tmd2771 driver");
MODULE_LICENSE("GPL");
