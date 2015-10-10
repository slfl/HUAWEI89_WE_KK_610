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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.c
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 *
 *
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 01 04 2012 hao.wang
 * [ALPS00109603] getsensorid func check in
 * .
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
//#include <mach/mt6516_pll.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"


#include "bf3905mipiyuv_Sensor.h"
#include "bf3905mipiyuv_Camera_Sensor_para.h"
#include "bf3905mipiyuv_CameraCustomized.h"

#define BF3905MIPIYUV_DEBUG
#ifdef BF3905MIPIYUV_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
kal_uint16 BF3905MIPI_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	char puSendCmd[2] = {(char)(addr & 0xFF),(char)(para & 0xFF)};

	iWriteRegI2C(puSendCmd,2,BF3905MIPI_WRITE_ID);

}

kal_uint16 BF3905MIPI_read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;
    char puSendCmd = {(char)(addr & 0xFF)};
	iReadRegI2C(&puSendCmd , 1, (u8*)&get_byte,1,BF3905MIPI_WRITE_ID);
    return get_byte;
}


/*******************************************************************************
* // Adapter for Winmo typedef 
********************************************************************************/
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)

struct BF3905_sensor_struct BF3905MIPI_Sensor_Driver;
MSDK_SENSOR_CONFIG_STRUCT BF3905MIPISensorConfigData;

//void sequencer_refresh()
//{
//	/* It is recommended that Refresh and Refresh mode commands always	run 
//	together, and Refresh mode should be issued before the Refresh command. */
//	BF3905MIPI_write_cmos_sensor( 0x098C, 0xA103 );    // MCU_ADDRESS
//	BF3905MIPI_write_cmos_sensor( 0x0990, 0x06 );    // MCU_DATA_0
//	Sleep(25);
//	BF3905MIPI_write_cmos_sensor( 0x098C, 0xA103 );    // MCU_ADDRESS
//	BF3905MIPI_write_cmos_sensor( 0x0990, 0x05 );    // MCU_DATA_0
//	Sleep(25);
//}

void BF3905MIPI_Initial_Setting(void)
{
    printk("BF3905MIPI_Initial_Setting!!!\n");

	BF3905MIPI_write_cmos_sensor(0x15,0x12);
	BF3905MIPI_write_cmos_sensor(0x3e,0x83);
	BF3905MIPI_write_cmos_sensor(0x09,0x03);
	BF3905MIPI_write_cmos_sensor(0x12,0x00);
	BF3905MIPI_write_cmos_sensor(0x3a,0x20);
	BF3905MIPI_write_cmos_sensor(0x1b,0x0e);
	BF3905MIPI_write_cmos_sensor(0x2a,0x00);
	BF3905MIPI_write_cmos_sensor(0x2b,0x10);
	BF3905MIPI_write_cmos_sensor(0x92,0x09);
	BF3905MIPI_write_cmos_sensor(0x93,0x00);
	BF3905MIPI_write_cmos_sensor(0x8a,0x96);
	BF3905MIPI_write_cmos_sensor(0x8b,0x7d);
	BF3905MIPI_write_cmos_sensor(0x13,0x00);
	BF3905MIPI_write_cmos_sensor(0x01,0x15);
	BF3905MIPI_write_cmos_sensor(0x02,0x23);
	BF3905MIPI_write_cmos_sensor(0x9d,0x20);
	BF3905MIPI_write_cmos_sensor(0x8c,0x02);
	BF3905MIPI_write_cmos_sensor(0x8d,0xee);
	BF3905MIPI_write_cmos_sensor(0x13,0x07);
	BF3905MIPI_write_cmos_sensor(0x5d,0xb3);
	BF3905MIPI_write_cmos_sensor(0xbf,0x08);
	BF3905MIPI_write_cmos_sensor(0xc3,0x08);
	BF3905MIPI_write_cmos_sensor(0xca,0x10);
	BF3905MIPI_write_cmos_sensor(0x62,0x00);
	BF3905MIPI_write_cmos_sensor(0x63,0x00);
	BF3905MIPI_write_cmos_sensor(0xb9,0x00);
	BF3905MIPI_write_cmos_sensor(0x64,0x00);
	BF3905MIPI_write_cmos_sensor(0xbb,0x10);
	BF3905MIPI_write_cmos_sensor(0x08,0x02);
	BF3905MIPI_write_cmos_sensor(0x20,0x09);
	BF3905MIPI_write_cmos_sensor(0x21,0x4f);
	BF3905MIPI_write_cmos_sensor(0x2f,0x84);
	BF3905MIPI_write_cmos_sensor(0x7e,0x84);
	BF3905MIPI_write_cmos_sensor(0x7f,0x3c);
	BF3905MIPI_write_cmos_sensor(0x60,0xe5);
	BF3905MIPI_write_cmos_sensor(0x61,0xf2);
	BF3905MIPI_write_cmos_sensor(0x6d,0xc0);
	BF3905MIPI_write_cmos_sensor(0x1e,0x40);
	BF3905MIPI_write_cmos_sensor(0xd9,0x25);
	BF3905MIPI_write_cmos_sensor(0xdf,0x26);
	BF3905MIPI_write_cmos_sensor(0x71,0x3f);
	BF3905MIPI_write_cmos_sensor(0x16,0xaf);
	BF3905MIPI_write_cmos_sensor(0x17,0x00);
	BF3905MIPI_write_cmos_sensor(0x18,0xa0);
	BF3905MIPI_write_cmos_sensor(0x19,0x00);
	BF3905MIPI_write_cmos_sensor(0x1a,0x78);
	BF3905MIPI_write_cmos_sensor(0x03,0x00);
	BF3905MIPI_write_cmos_sensor(0x4a,0x0c);
	BF3905MIPI_write_cmos_sensor(0xda,0x00);
	BF3905MIPI_write_cmos_sensor(0xdb,0xa2);
	BF3905MIPI_write_cmos_sensor(0xdc,0x00);
	BF3905MIPI_write_cmos_sensor(0xdd,0x7a);
	BF3905MIPI_write_cmos_sensor(0xde,0x00);
	BF3905MIPI_write_cmos_sensor(0x33,0x10);
	BF3905MIPI_write_cmos_sensor(0x34,0x08);
	BF3905MIPI_write_cmos_sensor(0x36,0xc5);
	BF3905MIPI_write_cmos_sensor(0x6e,0x20);
	BF3905MIPI_write_cmos_sensor(0xbc,0x0d);
	BF3905MIPI_write_cmos_sensor(0x35,0xb0);
	BF3905MIPI_write_cmos_sensor(0x65,0x9B);
	BF3905MIPI_write_cmos_sensor(0x66,0x9B);
	BF3905MIPI_write_cmos_sensor(0x70,0x0b);
	BF3905MIPI_write_cmos_sensor(0x71,0x0f);
	BF3905MIPI_write_cmos_sensor(0x72,0x4c);
	BF3905MIPI_write_cmos_sensor(0x73,0x27);
	BF3905MIPI_write_cmos_sensor(0x74,0x6d);
	BF3905MIPI_write_cmos_sensor(0x75,0x8a);
	BF3905MIPI_write_cmos_sensor(0x76,0x98);
	BF3905MIPI_write_cmos_sensor(0x77,0x2a);
	BF3905MIPI_write_cmos_sensor(0x78,0xff);
	BF3905MIPI_write_cmos_sensor(0x79,0x24);
	BF3905MIPI_write_cmos_sensor(0x7a,0x12);
	BF3905MIPI_write_cmos_sensor(0x7b,0x58);
	BF3905MIPI_write_cmos_sensor(0x7c,0x55);
	BF3905MIPI_write_cmos_sensor(0x7d,0x00);
	BF3905MIPI_write_cmos_sensor(0x13,0x07);
	BF3905MIPI_write_cmos_sensor(0x24,0x4c);//4a
	BF3905MIPI_write_cmos_sensor(0x25,0x88);
	BF3905MIPI_write_cmos_sensor(0x80,0x92);
	BF3905MIPI_write_cmos_sensor(0x81,0x00);
	BF3905MIPI_write_cmos_sensor(0x82,0x2a);
	BF3905MIPI_write_cmos_sensor(0x83,0x54);
	BF3905MIPI_write_cmos_sensor(0x84,0x39);
	BF3905MIPI_write_cmos_sensor(0x85,0x5d);
	BF3905MIPI_write_cmos_sensor(0x86,0x88);
	BF3905MIPI_write_cmos_sensor(0x89,0x63);
	BF3905MIPI_write_cmos_sensor(0x8e,0x2c);
	BF3905MIPI_write_cmos_sensor(0x8f,0x82);
	BF3905MIPI_write_cmos_sensor(0x94,0x42);
	BF3905MIPI_write_cmos_sensor(0x95,0x84);
	BF3905MIPI_write_cmos_sensor(0x96,0xb3);
	BF3905MIPI_write_cmos_sensor(0x97,0x4f);
	BF3905MIPI_write_cmos_sensor(0x98,0x8a);
	BF3905MIPI_write_cmos_sensor(0x99,0x10);
	BF3905MIPI_write_cmos_sensor(0x9a,0x50);
	BF3905MIPI_write_cmos_sensor(0x9f,0x64);
	BF3905MIPI_write_cmos_sensor(0x39,0x98);//90
	BF3905MIPI_write_cmos_sensor(0x3f,0x98);
	BF3905MIPI_write_cmos_sensor(0x90,0x20);
	BF3905MIPI_write_cmos_sensor(0x91,0xd0);
	BF3905MIPI_write_cmos_sensor(0x40,0x3b);
	BF3905MIPI_write_cmos_sensor(0x41,0x36);
	BF3905MIPI_write_cmos_sensor(0x42,0x2b);
	BF3905MIPI_write_cmos_sensor(0x43,0x1d);
	BF3905MIPI_write_cmos_sensor(0x44,0x1a);
	BF3905MIPI_write_cmos_sensor(0x45,0x14);
	BF3905MIPI_write_cmos_sensor(0x46,0x11);
	BF3905MIPI_write_cmos_sensor(0x47,0x0f);
	BF3905MIPI_write_cmos_sensor(0x48,0x0e);
	BF3905MIPI_write_cmos_sensor(0x49,0x0d);
	BF3905MIPI_write_cmos_sensor(0x4b,0x0c);
	BF3905MIPI_write_cmos_sensor(0x4c,0x0b);
	BF3905MIPI_write_cmos_sensor(0x4e,0x0a);
	BF3905MIPI_write_cmos_sensor(0x4f,0x09);
	BF3905MIPI_write_cmos_sensor(0x50,0x09);
	BF3905MIPI_write_cmos_sensor(0x5a,0x56);
	BF3905MIPI_write_cmos_sensor(0x51,0x13);
	BF3905MIPI_write_cmos_sensor(0x52,0x05);
	BF3905MIPI_write_cmos_sensor(0x53,0x91);
	BF3905MIPI_write_cmos_sensor(0x54,0x72);
	BF3905MIPI_write_cmos_sensor(0x57,0x96);
	BF3905MIPI_write_cmos_sensor(0x58,0x35);
	BF3905MIPI_write_cmos_sensor(0x5a,0xd6);
	BF3905MIPI_write_cmos_sensor(0x51,0x10);
	BF3905MIPI_write_cmos_sensor(0x52,0x06);
	BF3905MIPI_write_cmos_sensor(0x53,0x7a);
	BF3905MIPI_write_cmos_sensor(0x54,0x7d);
	BF3905MIPI_write_cmos_sensor(0x57,0x83);
	BF3905MIPI_write_cmos_sensor(0x58,0x15);
	BF3905MIPI_write_cmos_sensor(0x5b,0x02);
	BF3905MIPI_write_cmos_sensor(0x5c,0x26);
	BF3905MIPI_write_cmos_sensor(0x6a,0x81);
	BF3905MIPI_write_cmos_sensor(0x23,0x55);
	BF3905MIPI_write_cmos_sensor(0xa0,0x00);
	BF3905MIPI_write_cmos_sensor(0xa1,0x31);
	BF3905MIPI_write_cmos_sensor(0xa2,0x0d);
	BF3905MIPI_write_cmos_sensor(0xa3,0x27);
	BF3905MIPI_write_cmos_sensor(0xa4,0x0a);
	BF3905MIPI_write_cmos_sensor(0xa5,0x2c);
	BF3905MIPI_write_cmos_sensor(0xa6,0x04);
	BF3905MIPI_write_cmos_sensor(0xa7,0x1a);
	BF3905MIPI_write_cmos_sensor(0xa8,0x18);
	BF3905MIPI_write_cmos_sensor(0xa9,0x13);
	BF3905MIPI_write_cmos_sensor(0xaa,0x18);
	BF3905MIPI_write_cmos_sensor(0xab,0x1c);
	BF3905MIPI_write_cmos_sensor(0xac,0x3c);
	BF3905MIPI_write_cmos_sensor(0xad,0xf0);
	BF3905MIPI_write_cmos_sensor(0xae,0x57);
	BF3905MIPI_write_cmos_sensor(0xc5,0xaa);
	BF3905MIPI_write_cmos_sensor(0xc6,0x88);
	BF3905MIPI_write_cmos_sensor(0xc7,0x10);
	BF3905MIPI_write_cmos_sensor(0xc8,0x0d);
	BF3905MIPI_write_cmos_sensor(0xc9,0x10);
	BF3905MIPI_write_cmos_sensor(0xd0,0xb2);
	BF3905MIPI_write_cmos_sensor(0xd1,0x00);
	BF3905MIPI_write_cmos_sensor(0xd2,0x58);
	BF3905MIPI_write_cmos_sensor(0xd3,0x09);
	BF3905MIPI_write_cmos_sensor(0xd4,0x24);
	BF3905MIPI_write_cmos_sensor(0xee,0x30);
	BF3905MIPI_write_cmos_sensor(0xb0,0xe0);
	BF3905MIPI_write_cmos_sensor(0xb3,0x48);
	BF3905MIPI_write_cmos_sensor(0xb4,0xe3);
	BF3905MIPI_write_cmos_sensor(0xb1,0xff);
	BF3905MIPI_write_cmos_sensor(0xb2,0xff);
	BF3905MIPI_write_cmos_sensor(0xb4,0x63);
	BF3905MIPI_write_cmos_sensor(0xb1,0xb5);
	BF3905MIPI_write_cmos_sensor(0xb2,0xa5);
	BF3905MIPI_write_cmos_sensor(0x55,0x00);
	BF3905MIPI_write_cmos_sensor(0x56,0x40);

    Sleep(300);//DELAY=300
}

void BF3905MIPI_Init_Para(void)
{
	SENSORDB("[Enter]:BF3905_Init_Para\n");

	BF3905MIPI_Sensor_Driver.Preview_PClk = 26;// 12Mhz
	BF3905MIPI_Sensor_Driver.Dummy_Pixels = 0;
	BF3905MIPI_Sensor_Driver.Dummy_Lines= 0;
	BF3905MIPI_Sensor_Driver.Min_Frame_Rate = 80;
	BF3905MIPI_Sensor_Driver.Max_Frame_Rate = 300;

	//BF3905MIPI_Sensor_Driver.Preview_Pixels_In_Line = BF3905MIPI_DEFUALT_PREVIEW_LINE_LENGTH + BF3905MIPI_Sensor_Driver.Dummy_Pixels;
	//BF3905MIPI_Sensor_Driver.Preview_Lines_In_Frame = BF3905MIPI_DEFUALT_PREVIEW_FRAME_LENGTH + BF3905MIPI_Sensor_Driver.Dummy_Lines;
}

static kal_uint16 BF3905MIPI_power_on(void)
{
	BF3905MIPI_Sensor_Driver.sensor_id = 0;
	BF3905MIPI_Sensor_Driver.sensor_id = ((BF3905MIPI_read_cmos_sensor(0xfc)<<8)&0xff00)|(BF3905MIPI_read_cmos_sensor(0xfd)&0x00ff);


	SENSORDB("[BF3905MIPI]BF3905MIPI_Sensor_Driver.sensor_id =%x\n",BF3905MIPI_Sensor_Driver.sensor_id);
	return BF3905MIPI_Sensor_Driver.sensor_id;
}


/*************************************************************************
* FUNCTION
*	BF3905Open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 BF3905MIPIOpen(void)
{
     SENSORDB("[Enter]:BF3905 Open func zhijie:\n");
#if 1
	 if (BF3905MIPI_power_on() != BF3905MIPI_SENSOR_ID) 
	 	{
	 	   SENSORDB("[BF3905]Error:read sensor ID fail\n");
		   return ERROR_SENSOR_CONNECT_FAIL;
	 	}
    
 #endif
    /* Apply sensor initail setting*/
     BF3905MIPI_Initial_Setting();
     BF3905MIPI_Init_Para();
	 SENSORDB("[Exit]:BF3905 Open func\n");
     
	return ERROR_NONE;
}	/* BF3905Open() */


UINT32 BF3905MIPIGetSensorID(UINT32 *sensorID)
{
     SENSORDB("[Enter]:BF3905 BF3905MIPIGetSensorID func zhijie: \n");

	 *sensorID = BF3905MIPI_power_on();

	 if (*sensorID != BF3905MIPI_SENSOR_ID) 
	 	{
	 	   SENSORDB("[BF3905MIPI]Error:read sensor ID fail\n");
		   *sensorID = 0xFFFFFFFF;
		   return ERROR_SENSOR_CONNECT_FAIL;
	 	}
   
      
    /* Apply sensor initail setting*/
     //BF3905_Initial_Setting();
     //BF3905_Init_Para();
	 SENSORDB("[Exit]:BF3905MIPI BF3905MIPIGetSensorID func\n");

	return ERROR_NONE;
}	/* BF3905Open() */


/*************************************************************************
* FUNCTION
*	BF3905Close
*
* DESCRIPTION
*	This function is to turn off sensor module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 BF3905MIPIClose(void)
{

	return ERROR_NONE;
}	/* BF3905Close() */


static void BF3905MIPI_HVMirror(kal_uint8 image_mirror)
{
	switch (image_mirror)
	{
	case IMAGE_NORMAL:
		/* Preview image orientation */
		BF3905MIPI_write_cmos_sensor(0x1e, 0x40);
		break;
	case IMAGE_H_MIRROR:
		BF3905MIPI_write_cmos_sensor(0x1e, 0x60);
		break;
	case IMAGE_V_MIRROR:
		BF3905MIPI_write_cmos_sensor(0x1e, 0x50);
		break;
	case IMAGE_HV_MIRROR:
		BF3905MIPI_write_cmos_sensor(0x1e, 0x70);
		break;
	default:
		BF3905MIPI_write_cmos_sensor(0x1e, 0x40);
		break;
	}

}

/*****************************************************************************
* FUNCTION
*  set_preview_dummy
* DESCRIPTION
*  		For preview config the dummy pixel and dummy line
*
* PARAMETERS
*  H_Blank: The value of dummy pixels.
*  V_Blank: The value of dummy lines.
* RETURNS
*  
*****************************************************************************/
void BF3905MIPI_set_preview_dummy(kal_uint32 dummy_pixels, kal_uint32 dummy_lines)
{
	
	BF3905MIPI_write_cmos_sensor(0x2A, ((dummy_pixels&0xF00)>>4));
	BF3905MIPI_write_cmos_sensor(0x2B, (dummy_pixels&0xFF));
	BF3905MIPI_write_cmos_sensor(0x92, (dummy_lines&0xFF));
	BF3905MIPI_write_cmos_sensor(0x93, ((dummy_lines&0xFF00)>>8));

}


void BF3905MIPIFixFrameRate(kal_uint32 Min,kal_uint32 Max)
{

}

void BF3905MIPI_night_mode(kal_bool enable)
{

	  SENSORDB("[Enter]BF3905MIPI night mode func:enable = %d\n",enable);

		if(enable)
	{

		BF3905MIPI_write_cmos_sensor(0x86, 0xff); 
		BF3905MIPI_write_cmos_sensor(0x89, 0xa3);
	}
	else
    {
		BF3905MIPI_write_cmos_sensor(0x86, 0x88); 
		BF3905MIPI_write_cmos_sensor(0x89, 0x63);
	}
	
}

/*************************************************************************
* FUNCTION
*	BF3905Preview
*
* DESCRIPTION
*	This function start the sensor preview.
*
* PARAMETERS
*	*image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static UINT32 BF3905MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	
//	if(BF3905_sensor.first_init == KAL_TRUE)
//		BF3905_sensor.MPEG4_Video_mode = BF3905_sensor.MPEG4_Video_mode;
//	else
//		BF3905_sensor.MPEG4_Video_mode = !BF3905_sensor.MPEG4_Video_mode;

	
	SENSORDB("[Enter]:BF3905 preview func:");		

    BF3905MIPI_Sensor_Driver.Camco_mode = BF3905_CAM_PREVIEW;

	BF3905MIPI_Sensor_Driver.Preview_PClk = 26;// 12Mhz
	BF3905MIPI_Sensor_Driver.Dummy_Pixels = 0;
	BF3905MIPI_Sensor_Driver.Dummy_Lines= 0;
	BF3905MIPI_Sensor_Driver.Min_Frame_Rate = 80;
	BF3905MIPI_Sensor_Driver.Max_Frame_Rate = 300;
		

	BF3905MIPI_Sensor_Driver.StartX=1;
	BF3905MIPI_Sensor_Driver.StartY=1;
	BF3905MIPI_Sensor_Driver.iGrabWidth = BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH - 16;//16;
	BF3905MIPI_Sensor_Driver.iGrabheight = BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT - 12;//16;
   
	BF3905MIPI_HVMirror(sensor_config_data->SensorImageMirror);


	//BF3905MIPI_Sensor_Driver.Preview_Pixels_In_Line = BF3905MIPI_DEFUALT_PREVIEW_LINE_LENGTH + BF3905MIPI_Sensor_Driver.Dummy_Pixels;
	//BF3905MIPI_Sensor_Driver.Preview_Lines_In_Frame = BF3905MIPI_DEFUALT_PREVIEW_FRAME_LENGTH + BF3905MIPI_Sensor_Driver.Dummy_Lines;
	//BF3905MIPI_set_preview_dummy(BF3905MIPI_Sensor_Driver.Preview_Lines_In_Frame, BF3905MIPI_Sensor_Driver.Preview_Pixels_In_Line);

	image_window->GrabStartX = BF3905MIPI_Sensor_Driver.StartX;
	image_window->GrabStartY = BF3905MIPI_Sensor_Driver.StartY;
	image_window->ExposureWindowWidth = BF3905MIPI_Sensor_Driver.iGrabWidth;
	image_window->ExposureWindowHeight = BF3905MIPI_Sensor_Driver.iGrabheight;

	SENSORDB("[Exit]:BF3905 preview func\n");
    return ERROR_NONE; 
}	/* BF3905_Preview */

UINT32 BF3905MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	BF3905MIPI_Sensor_Driver.Camco_mode = BF3905_CAM_CAPTURE;

	BF3905MIPI_Sensor_Driver.StartX=1;
	BF3905MIPI_Sensor_Driver.StartY=1;
	BF3905MIPI_Sensor_Driver.iGrabWidth=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH - 16;
	BF3905MIPI_Sensor_Driver.iGrabheight=BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT - 12;

	image_window->GrabStartX = BF3905MIPI_Sensor_Driver.StartX;
	image_window->GrabStartY = BF3905MIPI_Sensor_Driver.StartY;
	image_window->ExposureWindowWidth = BF3905MIPI_Sensor_Driver.iGrabWidth;
	image_window->ExposureWindowHeight = BF3905MIPI_Sensor_Driver.iGrabheight;
}


UINT32 BF3905MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    SENSORDB("[Enter]:BF3905MIPI get Resolution func\n");
	
	pSensorResolution->SensorFullWidth=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH - 16;  
	pSensorResolution->SensorFullHeight=BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT - 12;
	pSensorResolution->SensorPreviewWidth=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH - 16;
	pSensorResolution->SensorPreviewHeight=BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT - 12;
        //ADD video resolution for MT6589
	pSensorResolution->SensorVideoWidth=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH - 16; //width
	pSensorResolution->SensorVideoHeight=BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT - 12; //height

    SENSORDB("[Exit]:BF3905 get Resolution func\n");
	
	return ERROR_NONE;
}	/* BF3905GetResolution() */

UINT32 BF3905MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    SENSORDB("[Enter]:BF3905 getInfo func:ScenarioId = %d\n",ScenarioId);
   
	pSensorInfo->SensorPreviewResolutionX=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH;
	pSensorInfo->SensorPreviewResolutionY=BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT;
	pSensorInfo->SensorFullResolutionX=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH;
	pSensorInfo->SensorFullResolutionY=BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=30;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;//low is to reset 
	pSensorInfo->SensorResetDelayCount=4;  //4ms 
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;	
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
	pSensorInfo->SensorInterruptDelayLines = 1; 
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_MIPI;

	pSensorInfo->CaptureDelayFrame = 1; 
	pSensorInfo->PreviewDelayFrame = 2; 
	pSensorInfo->VideoDelayFrame = 0; 
	pSensorInfo->SensorMasterClockSwitch = 0; 
       pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;   		
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
//		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			pSensorInfo->SensorClockFreq=26; //lichaofeng modified
			pSensorInfo->SensorClockDividCount=	7;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 4;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
	        pSensorInfo->SensorGrabStartX = 3; 
	        pSensorInfo->SensorGrabStartY = 2;  			
	            
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;			
	        pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
		    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
		    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	        pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
	        pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x 
	        pSensorInfo->SensorPacketECCOrder = 1;
			
		break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
//		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			pSensorInfo->SensorClockFreq=26;//lichaofeng modified
			pSensorInfo->SensorClockDividCount=	7;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 4;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = 3; 
            pSensorInfo->SensorGrabStartY = 2;			
	            
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;			
	        pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
		    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
		    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	        pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
	        pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x 
	        pSensorInfo->SensorPacketECCOrder = 1;
		break;
		default:
			pSensorInfo->SensorClockFreq=26;//lichaofeng modified
			pSensorInfo->SensorClockDividCount=7;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=4;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
            pSensorInfo->SensorGrabStartX = 4; 
            pSensorInfo->SensorGrabStartY = 2;  			
		break;
	}
//	BF3905_PixelClockDivider=pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &BF3905MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

	SENSORDB("[Exit]:BF3905MIPI getInfo func\n");
	
	return ERROR_NONE;
}	/* BF3905GetInfo() */


UINT32 BF3905MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
   SENSORDB("[Enter]:BF3905MIPI Control func:ScenarioId = %d\n",ScenarioId);

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
//		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			 BF3905MIPIPreview(pImageWindow, pSensorConfigData);
			 break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
//		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			 BF3905MIPICapture(pImageWindow, pSensorConfigData);
		default:
		     break; 
	}

   SENSORDB("[Exit]:BF3905MIPI Control func\n");
	
	return ERROR_NONE;
}	/* BF3905Control() */


/*************************************************************************
* FUNCTION
*	BF3905_set_param_wb
*
* DESCRIPTION
*	wb setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL BF3905MIPI_set_param_wb(UINT16 para)
{
	
    //This sensor need more time to balance AWB, 
    //we suggest higher fps or drop some frame to avoid garbage color when preview initial
   SENSORDB("[Enter]BF3905 set_param_wb func:para = %d\n",para);
	switch (para)
	{            
		case AWB_MODE_AUTO:
			{
				BF3905MIPI_write_cmos_sensor( 0x13, 0x07);	
            }
		    break;
		case AWB_MODE_CLOUDY_DAYLIGHT:
			{
		        BF3905MIPI_write_cmos_sensor( 0x13, 0x05);
				BF3905MIPI_write_cmos_sensor( 0x01, 0x10);
				BF3905MIPI_write_cmos_sensor( 0x02, 0x28);
	        }			   
		    break;
		case AWB_MODE_DAYLIGHT:
		    {
		        BF3905MIPI_write_cmos_sensor( 0x13, 0x05);
				BF3905MIPI_write_cmos_sensor( 0x01, 0x11);
				BF3905MIPI_write_cmos_sensor( 0x02, 0x26);
            }      
		    break;
		case AWB_MODE_INCANDESCENT:	
		    {
		        BF3905MIPI_write_cmos_sensor( 0x13, 0x05);
				BF3905MIPI_write_cmos_sensor( 0x01, 0x1f);
				BF3905MIPI_write_cmos_sensor( 0x02, 0x15);
            }		
		    break;  
		case AWB_MODE_FLUORESCENT:
		    {
		        BF3905MIPI_write_cmos_sensor( 0x13, 0x05);
				BF3905MIPI_write_cmos_sensor( 0x01, 0x1a);
				BF3905MIPI_write_cmos_sensor( 0x02, 0x1e);
            }	
		    break;  
		case AWB_MODE_TUNGSTEN:
		   {
		        BF3905MIPI_write_cmos_sensor( 0x13, 0x05);
				BF3905MIPI_write_cmos_sensor( 0x01, 0x1a);
				BF3905MIPI_write_cmos_sensor( 0x02, 0x0d);
		    break;
			}
		default:
			return FALSE;
	}

	return TRUE;
	
} /* BF3905_set_param_wb */

/*************************************************************************
* FUNCTION
*	BF3905_set_param_effect
*
* DESCRIPTION
*	effect setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL BF3905MIPI_set_param_effect(UINT16 para)
{

   SENSORDB("[Enter]BF3905 set_param_effect func:para = %d\n",para);
   switch (para)
	{
        case MEFFECT_OFF:
		{
            BF3905MIPI_write_cmos_sensor(0x70, 0x0b);
            BF3905MIPI_write_cmos_sensor(0x69, 0x00);
            BF3905MIPI_write_cmos_sensor(0x67, 0x80);
            BF3905MIPI_write_cmos_sensor(0x68, 0x80);
            BF3905MIPI_write_cmos_sensor(0xb4, 0x03);
            BF3905MIPI_write_cmos_sensor(0x56, 0x40);
        }
            break;
        case MEFFECT_NEGATIVE:
        {
            BF3905MIPI_write_cmos_sensor(0x70, 0x0b);
            BF3905MIPI_write_cmos_sensor(0x69, 0x01);
            BF3905MIPI_write_cmos_sensor(0x67, 0x80);
            BF3905MIPI_write_cmos_sensor(0x68, 0x80);
            BF3905MIPI_write_cmos_sensor(0xb4, 0x03);
            BF3905MIPI_write_cmos_sensor(0x56, 0x40);
        }
            break;
		case MEFFECT_SEPIA:
		{
            BF3905MIPI_write_cmos_sensor(0x70, 0x0b);
			BF3905MIPI_write_cmos_sensor(0x69, 0x20);
			BF3905MIPI_write_cmos_sensor(0x67, 0x60);
			BF3905MIPI_write_cmos_sensor(0x68, 0xa0);
			BF3905MIPI_write_cmos_sensor(0xb4, 0x03);
			BF3905MIPI_write_cmos_sensor(0x56, 0x40); 
        }
			break;  
		case MEFFECT_SEPIAGREEN:		
		{
            BF3905MIPI_write_cmos_sensor(0x70, 0x0b);
			BF3905MIPI_write_cmos_sensor(0x69, 0x20);
			BF3905MIPI_write_cmos_sensor(0x67, 0xe0);
			BF3905MIPI_write_cmos_sensor(0x68, 0x60);
			BF3905MIPI_write_cmos_sensor(0xb4, 0x03);
			BF3905MIPI_write_cmos_sensor(0x56, 0x40); 
        }
			break;
		case MEFFECT_SEPIABLUE:
		{
            BF3905MIPI_write_cmos_sensor(0x70, 0x0b);
			BF3905MIPI_write_cmos_sensor(0x69, 0x20);
			BF3905MIPI_write_cmos_sensor(0x67, 0xe0);
			BF3905MIPI_write_cmos_sensor(0x68, 0x60);
			BF3905MIPI_write_cmos_sensor(0xb4, 0x03);
			BF3905MIPI_write_cmos_sensor(0x56, 0x40); 
	    }
			break;        
		case MEFFECT_MONO:			
		{
            BF3905MIPI_write_cmos_sensor(0x70, 0x0b);
			BF3905MIPI_write_cmos_sensor(0x69, 0x20);
			BF3905MIPI_write_cmos_sensor(0x67, 0x80);
			BF3905MIPI_write_cmos_sensor(0x68, 0x80);
			BF3905MIPI_write_cmos_sensor(0xb4, 0x03);
			BF3905MIPI_write_cmos_sensor(0x56, 0x40); 
        }
			break;

		default:
			return KAL_FALSE;
	}

	return KAL_TRUE;

} /* BF3905_set_param_effect */

/*************************************************************************
* FUNCTION
*	BF3905_set_param_banding
*
* DESCRIPTION
*	banding setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL BF3905MIPI_set_param_banding(UINT16 para)
{


	SENSORDB("[Enter]BF3905 set_param_banding func:para = %d\n",para);
	switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
	    {
			BF3905MIPI_Sensor_Driver.Banding = AE_FLICKER_MODE_50HZ;
			BF3905MIPI_write_cmos_sensor(0x80, 0x92);			
			BF3905MIPI_write_cmos_sensor(0x8a, 0xA5);
	    }
		break;

		case AE_FLICKER_MODE_60HZ:
	    {
			BF3905MIPI_Sensor_Driver.Banding = AE_FLICKER_MODE_60HZ;
			BF3905MIPI_write_cmos_sensor(0x80, 0x90);
			BF3905MIPI_write_cmos_sensor(0x8b, 0x8a);
	    }
		break;

	    default:
        return KAL_FALSE;
	}



	return KAL_TRUE;
} /* BF3905_set_param_banding */




/*************************************************************************
* FUNCTION
*	BF3905_set_param_exposure
*
* DESCRIPTION
*	exposure setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL BF3905MIPI_set_param_exposure(UINT16 para)
{

	kal_uint16 base_target = 0;

	SENSORDB("[Enter]BF3905MIPI set_param_exposure func:para = %d\n",para);

	switch (para)
	{
		case AE_EV_COMP_13:  //+4 EV
			base_target = 0x40;
			break;  
		case AE_EV_COMP_10:  //+3 EV
			base_target = 0x38;
			break;    
		case AE_EV_COMP_07:  //+2 EV
			base_target = 0x20;
			break;    
		case AE_EV_COMP_03:	 //	+1 EV	
			base_target = 0x10;
			break;    
		case AE_EV_COMP_00:  // +0 EV
			base_target = 0x00;
			break;    
		case AE_EV_COMP_n03:  // -1 EV
			base_target = 0x90;
			break;    
		case AE_EV_COMP_n07:	// -2 EV
			base_target = 0xa0;
			break;    
		case AE_EV_COMP_n10:   //-3 EV
			base_target = 0xb0;
			break;
		case AE_EV_COMP_n13:  // -4 EV
			base_target = 0xc0;
			break;
		default:
			return FALSE;
	}

	BF3905MIPI_write_cmos_sensor(0x55, base_target);

	return TRUE;
	
} /* BF3905_set_param_exposure */


UINT32 BF3905MIPIYUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{
    SENSORDB("[Enter]BF3905YUVSensorSetting func:cmd = %d\n",iCmd);
	
	switch (iCmd) {
	case FID_SCENE_MODE:	    //auto mode or night mode

		    if (iPara == SCENE_MODE_OFF)//auto mode
		    {
		        BF3905MIPI_night_mode(FALSE); 
		    }
		    else if (iPara == SCENE_MODE_NIGHTSCENE)//night mode
		    {
	            BF3905MIPI_night_mode(TRUE); 
		    }	
			
	     break; 	    
	case FID_AWB_MODE:
           BF3905MIPI_set_param_wb(iPara);
	     break;
	case FID_COLOR_EFFECT:
           BF3905MIPI_set_param_effect(iPara);
	     break;
	case FID_AE_EV:	    	    
           BF3905MIPI_set_param_exposure(iPara);
	     break;
	case FID_AE_FLICKER:	    	    	    
           BF3905MIPI_set_param_banding(iPara);
	     break;
	case FID_ZOOM_FACTOR:
	     BF3905MIPI_Sensor_Driver.Digital_Zoom_Factor= iPara;
	     break; 
	default:
	     break;
	}
	return TRUE;
}   /* BF3905YUVSensorSetting */

UINT32 BF3905MIPIYUVSetVideoMode(UINT16 u2FrameRate)
{
    SENSORDB("[Enter]BF3905 Set Video Mode:FrameRate= %d\n",u2FrameRate);

	if (u2FrameRate == 30)
    {
    	
		BF3905MIPI_Sensor_Driver.Min_Frame_Rate = 270;
		BF3905MIPI_Sensor_Driver.Max_Frame_Rate = 270;
    }
    else if (u2FrameRate == 15)       
    {
                
		BF3905MIPI_Sensor_Driver.Min_Frame_Rate = 150;
		BF3905MIPI_Sensor_Driver.Max_Frame_Rate = 150;
    }
    else 
    {
        printk("Wrong frame rate setting \n");
    }   



    return TRUE;
}

UINT32 BF3905MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 u2Temp = 0; 
	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH;
			*pFeatureReturnPara16=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_GET_PERIOD:
			*pFeatureReturnPara16++=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH+BF3905MIPI_Sensor_Driver.Dummy_Pixels;
			*pFeatureReturnPara16=BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH+BF3905MIPI_Sensor_Driver.Dummy_Lines;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			//*pFeatureReturnPara32 = BF3905_sensor_pclk/10;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_SET_ESHUTTER:
	
		     break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
			 BF3905MIPI_night_mode((BOOL) *pFeatureData16);
		     break;
		case SENSOR_FEATURE_SET_GAIN:
			 break; 
		case SENSOR_FEATURE_SET_FLASHLIGHT:
		     break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		     break;
		case SENSOR_FEATURE_SET_REGISTER:
			 BF3905MIPI_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
		     break;
		case SENSOR_FEATURE_GET_REGISTER:
			 pSensorRegData->RegData = BF3905MIPI_read_cmos_sensor(pSensorRegData->RegAddr);
		     break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			 memcpy(pSensorConfigData, &BF3905MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			 *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
		     break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
		     break;
		case SENSOR_FEATURE_GET_GROUP_COUNT:
	               // *pFeatureReturnPara32++=0;
			//*pFeatureParaLen=4;
		     break; 

		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			// get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
			// if EEPROM does not exist in camera module.
			*pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_SET_YUV_CMD:
			 BF3905MIPIYUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
		     break;	
		case SENSOR_FEATURE_SET_VIDEO_MODE:
		     BF3905MIPIYUVSetVideoMode(*pFeatureData16);
		     break; 
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
             BF3905MIPIGetSensorID(pFeatureReturnPara32); 
            break;     
		default:
			 break;
	}
	return ERROR_NONE;
}	/* BF3905FeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncBF3905MIPI=
{
	BF3905MIPIOpen,
	BF3905MIPIGetInfo,
	BF3905MIPIGetResolution,
	BF3905MIPIFeatureControl,
	BF3905MIPIControl,
	BF3905MIPIClose
};

UINT32 BF3905MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncBF3905MIPI;

	return ERROR_NONE;
}	/* SensorInit() */

