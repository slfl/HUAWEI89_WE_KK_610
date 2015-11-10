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


#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>
#include <linux/kernel.h>//for printk


#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"
#include <linux/hardware_self_adapt.h>

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg)

#define DEBUG_CAMERA_HW_K
#ifdef DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         xlog_printk(ANDROID_LOG_ERR, PFX , fmt, ##arg)
#define PK_XLOG_INFO(fmt, args...) \
                do {    \
                   xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg); \
                } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif

int kdCISModulePowerOnG700(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name);

int kdCISModulePowerOnG610(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name);

/* We use DIFFERENT power on/off seq on DIFFRENT product according to board ID.*/
int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
   /* hw_product_type boardType;
    boardType = get_hardware_product_main_version();

    if (HW_G700_VER == boardType)
    {
        PK_DBG("[CAMERA SENSOR] This machine is G700T.\n");
        return kdCISModulePowerOnG700(SensorIdx,currSensorName,On,mode_name);
    }

    if (HW_G610U_VER == boardType)
    {*/
        PK_DBG("[CAMERA SENSOR] This machine is G610U.\n");
        return kdCISModulePowerOnG610(SensorIdx,currSensorName,On,mode_name);
    /*}

    if (HW_G700U_VER == boardType)
    {
        PK_DBG("[CAMERA SENSOR] This machine is G700U.\n");
        return kdCISModulePowerOnG700(SensorIdx,currSensorName,On,mode_name);
    }
    else
    {
        PK_DBG("[CAMERA SENSOR] ERROR!Cannot identify  machine type!!\n");
         return -ENXIO;
    }*/
}

int kdCISModulePowerOnG610(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{

#define S5K4E1 1
#define OV5647 2
#define IDNONE 4
    static int cameraType = IDNONE;

    printk("[kdCISModulePowerOnG610 SensorIdx = %d, On =%d \n",SensorIdx,On);

    //power ON
    if (On) {

        if(DUAL_CAMERA_SUB_SENSOR == SensorIdx)
        {

            // Power on for sub sensor
            printk("Power on for G610 subsensor: currSensorName:%s\n",currSensorName);

            //Power on AF power to avoid ov5647 to pull down I2C
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
            printk("Set main pwd pin to zero !\n");


            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

            //enable sub sensor--PWD pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IOVDD=1.8 V
            {
              PK_DBG("[CAMERA SENSOR] Fail to enable IO power\n");
              goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD=2.8v
            {
              PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
              goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_DVDD_SUB, VOL_1800,mode_name))//DVDD=1.8V  //f00208919 added
            {
              PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
              goto _kdCISModulePowerOn_exit_;
            }
 
            mdelay(1);
            //enable sub sensor--RESET pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
            mdelay(2);
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

            mdelay(10);

        }
        else if ( DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {
            printk("Power on for G610  mainsensor: currSensorName:%s\n",currSensorName);

            if ( currSensorName && (0 == strcmp(currSensorName,"s5k4e1gamipiraw")))
            {
                printk("G610 Power on for  : s5k4e1gamipiraw:\n");
                cameraType = S5K4E1;
                //disable sub sensor --Power down pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down
                
                //disable sub sensor--RESET pin.
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))//DVDD=1.8V
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD=2.8v
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IOVDD=1.8 V
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable IO power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))//AF power
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                mdelay(5); 

                //enable main sensor---RESET pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
                msleep(10);
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                mdelay(2);

                //enable AF --PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN, GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(12);

            }
            else if (currSensorName && (0 == strcmp(currSensorName,"ov5647mipiraw")))
            {

                printk("G610 Power on  for : ov5647:\n");
                cameraType = OV5647;

                //Power on AF power to avoid ov5647 to pull down I2C
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                //disable sub sensor --Power down pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

                //disable sub sensor--RESET pin.
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                 if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IOVDD=1.8 V
                 {
                     PK_DBG("[CAMERA SENSOR] Fail to enable IO power\n");
                     goto _kdCISModulePowerOn_exit_;
                 }

                 if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD=2.8v
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))//DVDD=1.8V
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                mdelay(10);
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(5);   
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
                mdelay(5);

                //enable AF --PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN,GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                mdelay(20);
            }
            else
            {
                PK_DBG("[CAMERA SENSOR] Cannot identify sensor name!\n");
                cameraType = IDNONE;
            }
        }

    }
    else {//power OFF

        if(DUAL_CAMERA_SUB_SENSOR == SensorIdx)
        {
            printk("Power off for G610 subsensor: currSensorName:%s\n",currSensorName);

            //disable sub sensor--PWD pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

            //disable sub sensor--RST pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
            mdelay(2);

     
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_DVDD_SUB, mode_name)) {//DVDD=1.8V
                PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {//AVDD=2.8v
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                goto _kdCISModulePowerOn_exit_;
             }
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name)){//IOVDD=1.8V
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

        }
        else if ( DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {
            printk("Power off for mainsensor \n");
            printk("Power off for G610 mainsensor: currSensorName:%s\n",currSensorName);
            if (S5K4E1 == cameraType)
            {
                 printk("Power off   for : S5K4E1:\n");

                //disable sub sensor--PWD pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

                //disable sub sensor--RST pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(2);


                //Disable AF--PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN,GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                //power down main sensor
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {//AVDD=2.8v
                    PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                 }
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {//DVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name)){//IOVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF  CAMERA_POWER_VCAM_A2 power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

            }
            else if (OV5647 == cameraType)
            {
                printk("Power off   for : ov5647mip:\n");

                //disable sub sensor--PWD pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

                //disable sub sensor--RST pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(2);

                //Disable AF--PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN,GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}


                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                 if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                 if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                 if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {//DVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to OFF CAMERA_POWER_VCAM_D power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {//AVDD=2.8v
                    PK_DBG("[CAMERA SENSOR] Fail to OFF CAMERA_POWER_VCAM_A power\n");
                    goto _kdCISModulePowerOn_exit_;
                 }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name)){//IOVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to OFF CAMERA_POWER_VCAM_D2 power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF  CAMERA_POWER_VCAM_A2 power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

            }

        }
        mdelay(10);
    }

    return 0;
_kdCISModulePowerOn_exit_:
    return -EIO;
	
}

int kdCISModulePowerOnG700(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{

#define S5K4H5 1
#define OV8850 2
#define IDNONE 4
    static int cameraType = IDNONE;

    //power ON
    if (On) 
    {

        PK_DBG("kdCISModulePowerOnG700 -on:currSensorName=%s\n",currSensorName);

        if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx)//main camera
        {
            PK_DBG("kdCISModulePowerOnG700 -Power On Main, currSensorName=%s\n",currSensorName);

            if ( currSensorName && (0 == strcmp(currSensorName,"s5k4h5yxmipiraw")))
            {
                PK_DBG("kdCISModulePowerOnG700: Power on for s5k4h5yxmipiraw");
                cameraType = S5K4H5;

                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                //disable sub
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                //MAIN ON AVDD 2.8
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

                //MIAN ON DVDD 1.2
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1200,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                //MIAN ON DOVDD 1.8
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog AF power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                msleep(5);

                //RST pin
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
                msleep(10);
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
                msleep(1);

                //enable AF --PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN, GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(12);

            }
            else if (currSensorName && (0 == strcmp(currSensorName,"ov8850mipiraw")))
            {
                PK_DBG("kdCISModulePowerOnG700: Power on for ov8850mipiraw");
                cameraType = OV8850;

                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

               //disable sub
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                //MAIN ON AVDD 2.8
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                 //MIAN ON DOVDD 1.8
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                //MIAN ON DVDD 1.2
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1200,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog AF power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

                mdelay(15);

                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN, GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(12);

            }
            else
            {
                PK_DBG("[CAMERA SENSOR] Cannot identify sensor name!\n");
                cameraType = IDNONE;
            }

        }
        else if(DUAL_CAMERA_SUB_SENSOR  == SensorIdx) //sub camera
        {

            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

            //SUB ON  DVDD DOVDD 1.8
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800/*VOL_1800*/,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
            //SUB ON  DVDD DOVDD 1.8
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800/*VOL_1800*/,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            //SUB ON AVDD 2.8
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            msleep(20);

            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            msleep(10);
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}


            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            mdelay(50);
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            mdelay(2);


        }
        else
        {
            goto _kdCISModulePowerOn_exit_;
        }

    }
    else 
    {//power OFF

        PK_DBG("kdCISModulePowerOn -off:currSensorName=%s\n",currSensorName);

        if(DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {
            PK_DBG("kdCISModulePowerOn -power off main:currSensorName=%s\n",currSensorName);

            if (S5K4H5 == cameraType)
            {
                PK_DBG("kdCISModulePowerOn -power off main:S5K4H5 \n");

                //reset pull down
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                //disable AF --PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN, GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                //MAIN OFF DVDD ZHIBIN 
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) 
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                //MAIN AVDD
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) 
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                //DOVDD
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                //AF
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

            }
            else if (OV8850 == cameraType )
            {
                PK_DBG("kdCISModulePowerOn -power off main:OV8850 \n");
                //reset pull down
                 if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                 if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                 if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN, GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                //MAIN OFF DVDD ZHIBIN 
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) 
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }
                //DOVDD
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

                //MAIN AVDD
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) 
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

                //AF
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    //return -EIO;
                    goto _kdCISModulePowerOn_exit_;
                }

            }

        }

        else if(DUAL_CAMERA_SUB_SENSOR  == SensorIdx)
        {
            PK_DBG("kdCISModulePower off for sub sensor  \n");

            //reset pull down
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

            //PDN pull down
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

            // SUB OFF DVDD DOVDD  ZHBIN 

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) 
            {
            PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
            //return -EIO;
            goto _kdCISModulePowerOn_exit_;
            }
            //SUB OFF DVDD 1.8  ZHIBIN 
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D,mode_name))
            {
            PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
            //return -EIO;
            goto _kdCISModulePowerOn_exit_;
            }

            //SUB OFF AVDD 1.8  ZHIBIN 
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
            PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
            //return -EIO;
            goto _kdCISModulePowerOn_exit_;
            }

        }
        else
        {

            goto _kdCISModulePowerOn_exit_;
        }
    }
    return 0;

_kdCISModulePowerOn_exit_:
    return -EIO;
}

EXPORT_SYMBOL(kdCISModulePowerOn);

//!--
//





