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
 *   sensor.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Header file of Sensor driver
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
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/* SENSOR FULL SIZE */
#ifndef __SENSOR_H
#define __SENSOR_H

/* Sensor QVGA size */
#define IMAGE_SENSOR_QVGA_WIDTH					(320)
#define IMAGE_SENSOR_QVGA_HEIGHT				(240)
/* SENSOR VGA SIZE */
#define BF3905MIPI_IMAGE_SENSOR_VGA_WIDTH					(640)
#define BF3905MIPI_IMAGE_SENSOR_VGA_HEIGHT					(480)
/* SENSOR SXGA SIZE */
#define IMAGE_SENSOR_SXGA_WIDTH             	(1280)
#define IMAGE_SENSOR_SXGA_HEIGHT            	(1024)

/* Sensor vertical blanking & horizontal blanking, for preview and capture mode. */
#define BF3905MIPI_DEFUALT_PREVIEW_LINE_LENGTH  0x0310  //842
#define BF3905MIPI_DEFUALT_PREVIEW_FRAME_LENGTH 0x01FE  //518

#define PREVIEW_VISIBLE_PIXELS				(0x28B - 0x04)
#define PREVIEW_VISIBLE_LINES				(0x1EB - 0x04)
#define CAPTURE_VISIBLE_PIXELS				(0x28B - 0x04)
#define CAPTURE_VISIBLE_LINES				(0x1EB - 0x04)


#define PREVIEW_H_BLANKING					(0x034A - PREVIEW_VISIBLE_PIXELS)
#define PREVIEW_V_BLANKING					(0x0206/*0x0276*/ - PREVIEW_VISIBLE_LINES)
#define CAPTURE_H_BLANKING					(0x06FE - CAPTURE_VISIBLE_PIXELS)
#define CAPTURE_V_BLANKING					(0x01FB - CAPTURE_VISIBLE_LINES)


/* Flicker factor to calculate tha minimal shutter width step for 50Hz and 60Hz  */
#define MACRO_50HZ							(100)
#define MACRO_60HZ							(120)

#define FACTOR_50HZ							(MACRO_50HZ * 1000)
#define FACTOR_60HZ							(MACRO_60HZ * 1000)


//Mode state
typedef enum BF3905_CAMCO_MODE
{		
  BF3905_CAM_PREVIEW=0,//Camera Preview
  
  BF3905_CAM_CAPTURE,//Camera Capture

  BF3905_VIDEO_MPEG4,//Video Mode
  BF3905_VIDEO_MJPEG,
  
  BF3905_WEBCAM_CAPTURE,//WebCam
  
  BF3905_VIDEO_MAX
} BF3905_Camco_MODE;

struct BF3905_sensor_struct
{
	kal_uint16 sensor_id;

	kal_uint16 Dummy_Pixels;
	kal_uint16 Dummy_Lines;
	kal_uint32 Preview_PClk;

	kal_uint32 Preview_Lines_In_Frame;  
	kal_uint32 Capture_Lines_In_Frame;

	kal_uint32 Preview_Pixels_In_Line;  
	kal_uint32 Capture_Pixels_In_Line;
	kal_uint16 Preview_Shutter;
	kal_uint16 Capture_Shutter;

	kal_uint16 StartX;
	kal_uint16 StartY;
	kal_uint16 iGrabWidth;
	kal_uint16 iGrabheight;

	kal_uint16 Capture_Size_Width;
	kal_uint16 Capture_Size_Height;
	kal_uint32 Digital_Zoom_Factor;

	kal_uint16 Max_Zoom_Factor;

	kal_uint32 Min_Frame_Rate;
	kal_uint32 Max_Frame_Rate;
	kal_uint32 Fixed_Frame_Rate;
	//kal_bool Night_Mode;
	BF3905_Camco_MODE Camco_mode;
	AE_FLICKER_MODE_T Banding;

	kal_bool Night_Mode;
};

  
#define BF3905MIPI_WRITE_ID 0xDC
#define BF3905MIPI_READ_ID 0xdd


//export functions
UINT32 BF3905MIPIOpen(void);
UINT32 BF3905MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 BF3905MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 BF3905MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 BF3905MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 BF3905MIPIClose(void);


#endif /* __SENSOR_H */

