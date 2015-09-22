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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_ov5647mipiraw.h"
#include "camera_info_ov5647mipiraw.h"
#include "camera_custom_AEPlinetable.h"

const NVRAM_CAMERA_ISP_PARAM_STRUCT CAMERA_ISP_DEFAULT_VALUE =
{{
    //Version
    Version: NVRAM_CAMERA_PARA_FILE_VERSION,

    //SensorId
    SensorId: SENSOR_ID,
    ISPComm:{
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    	}
    },
    ISPPca: {
        #include INCLUDE_FILENAME_ISP_PCA_PARAM
    },
    ISPRegs:{
        #include INCLUDE_FILENAME_ISP_REGS_PARAM
    },
    ISPMfbMixer:{{
        {//00: MFB mixer for ISO 100
            0x00000000, 0x00000000
        },
        {//01: MFB mixer for ISO 200
            0x00000000, 0x00000000
        },
        {//02: MFB mixer for ISO 400
            0x00000000, 0x00000000
        },
        {//03: MFB mixer for ISO 800
            0x00000000, 0x00000000
        },
        {//04: MFB mixer for ISO 1600
            0x00000000, 0x00000000
        },
        {//05: MFB mixer for ISO 2400
            0x00000000, 0x00000000
        },
        {//06: MFB mixer for ISO 3200
            0x00000000, 0x00000000
        }
    }},
    ISPCcmPoly22:{
        73555, // i4R_AVG
        16178, // i4R_STD
        87896, // i4B_AVG
        20463, // i4B_STD
        { // i4P00[9]
            4725000, -2070000, -95000, -235000, 2745000, 50000,  315000, -1680000, 3925000 
        },
        { // i4P10[9]
            160240,  -153273, -6967, 118438,  -118438,  0, 61594,   -125405, 63811
        },
        { // i4P01[9]
            54369,  -52005, -2364, 40186, -40186,  0, -87793,  -42550,  130343
        },
        { // i4P20[9]
            0,  0,   0,  0,   0,  0, 0,  0,  0
        },
        { // i4P11[9]
            0,  0,   0,  0,   0,  0, 0,  0,  0
        },
        { // i4P02[9]
            0,  0,   0,  0,   0,  0, 0,  0,  0
        }        
    }
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        // rDevicesInfo
        {
            1136,   // u4MinGain, 1024 base =  1x
            8192,  // u4MaxGain, 16x  //
            106,     // u4MiniISOGain, ISOxx
            128,    // u4GainStepUnit, 1x/8
            34,     // u4PreExpUnit
            30,     // u4PreMaxFrameRate
            30,     // u4VideoExpUnit
            30,     // u4VideoMaxFrameRate
            1024,   // u4Video2PreRatio, 1024 base = 1x
            35,     // u4CapExpUnit
            14,     // u4CapMaxFrameRate
            1024,   // u4Cap2PreRatio, 1024 base = 1x
            28,      // u4LensFno, Fno = 2.8
            350     // u4FocusLength_100x
         },
         // rHistConfig
        {
            2,   // u4HistHighThres
            40,  // u4HistLowThres
            2,   // u4MostBrightRatio
            1,   // u4MostDarkRatio
            160, // u4CentralHighBound
            20,  // u4CentralLowBound
            {240, 230, 220, 210, 200}, // u4OverExpThres[AE_CCT_STRENGTH_NUM]
            {86, 108, 128, 140, 170},  // u4HistStretchThres[AE_CCT_STRENGTH_NUM]
            {18, 22, 26, 30, 34}       // u4BlackLightThres[AE_CCT_STRENGTH_NUM]
        },
        // rCCTConfig
        {
            TRUE,            // bEnableBlackLight
            TRUE,            // bEnableHistStretch
            FALSE,           // bEnableAntiOverExposure
            TRUE,            // bEnableTimeLPF
            FALSE,            // bEnableCaptureThres  //fix flare
            TRUE,            // bEnableVideoThres
            TRUE,            // bEnableStrobeThres
            47,                // u4AETarget
            65,                // u4StrobeAETarget  //47-->65

            20,                // u4InitIndex
            4,                 // u4BackLightWeight
            32,                // u4HistStretchWeight
            4,                 // u4AntiOverExpWeight
            3,                 // u4BlackLightStrengthIndex
            3,                 // u4HistStretchStrengthIndex
            2,                 // u4AntiOverExpStrengthIndex
            2,                 // u4TimeLPFStrengthIndex
            {1, 3, 5, 7, 8}, // u4LPFConvergeTable[AE_CCT_STRENGTH_NUM]
            90,                // u4InDoorEV = 9.0, 10 base
            -13,               // i4BVOffset delta BV = -2.3 //-23
            64,                 // u4PreviewFlareOffset   //12bit
            64,                 // u4CaptureFlareOffset  //12bit
            5,                 // u4CaptureFlareThres
            64,                 // u4VideoFlareOffset   //12bit
            5,                 // u4VideoFlareThres
            32,                 // u4StrobeFlareOffset  //12bit
            2,                 // u4StrobeFlareThres
            8,                 // u4PrvMaxFlareThres
            0,                 // u4PrvMinFlareThres
            8,                 // u4VideoMaxFlareThres
            0,                 // u4VideoMinFlareThres            
            18,                // u4FlatnessThres              // 10 base for flatness condition.
            75                 // u4FlatnessStrength
         }
    },

    // AWB NVRAM
    {
    	// AWB calibration data
    	{
    		// rUnitGain (unit gain: 1.0 = 512)
    		{
    			0,	// i4R
    			0,	// i4G
    			0	// i4B
    		},
    		// rGoldenGain (golden sample gain: 1.0 = 512)
    		{
	            0,	// i4R
	            0,	// i4G
	            0	// i4B
            },
    		// rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
    		{
	            0,	// i4R
	            0,	// i4G
	            0	// i4B
            },
            // rD65Gain (D65 WB gain: 1.0 = 512)
            {
                904,    // i4R
	           512,	// i4G
                589    // i4B
    		}
    	},
    	// Original XY coordinate of AWB light source
    	{
		    // Strobe
		    {
                158,    // i4X
                -262    // i4Y
		    },
    		// Horizon
    		{
                -393,    // i4X
                -322    // i4Y
    		},
    		// A
    		{
                -260,    // i4X
                -319    // i4Y
    		},
    		// TL84
    		{
                -94,    // i4X
                -325    // i4Y
    		},
    		// CWF
    		{
                -62,    // i4X
                -401    // i4Y
    		},
    		// DNP
    		{
                31,    // i4X
                -297    // i4Y
    		},
    		// D65
    		{
                158,    // i4X
                -262    // i4Y
    		},
		// DF
		{
			0,	// i4X
			0	// i4Y
    		}
    	},
    	// Rotated XY coordinate of AWB light source
    	{
		    // Strobe
		    {
                120,    // i4X
                -282    // i4Y
		    },
    		// Horizon
    		{
                -435,    // i4X
                -264    // i4Y
    		},
    		// A
    		{
                -303,    // i4X
                -280    // i4Y
    		},
    		// TL84
    		{
                -139,    // i4X
                -309    // i4Y
    		},
    		// CWF
    		{
                -118,    // i4X
                -389    // i4Y
    		},
    		// DNP
    		{
                -11,    // i4X
                -299    // i4Y
    		},
    		// D65
    		{
                120,    // i4X
                -282    // i4Y
    		},
		// DF
		{
			0,	// i4X
			0	// i4Y
    		}
    	},
	// AWB gain of AWB light source
	{
		// Strobe
		{
                904,    // i4R
			512,	// i4G
                589    // i4B
		},
		// Horizon
		{
			512,	// i4R
                564,    // i4G
                1486    // i4B
		},
		// A
		{
                555,    // i4R
			512,	// i4G
                1121    // i4B
		},
		// TL84
		{
                701,    // i4R
			512,	// i4G
                903    // i4B
		},
		// CWF
		{
                810,    // i4R
			512,	// i4G
                959    // i4B
		},
		// DNP
		{
                798,    // i4R
			512,	// i4G
                733    // i4B
		},
		// D65
		{
                904,    // i4R
			512,	// i4G
                589    // i4B
		},
		// DF
		{
			512,	// i4R
			512,	// i4G
			512     // i4B
		}
	},
    	// Rotation matrix parameter
    	{
            8,    // i4RotationAngle
            254,    // i4Cos
            36    // i4Sin
    	},
    	// Daylight locus parameter
    	{
            -167,    // i4SlopeNumerator
    		128	// i4SlopeDenominator
    	},
    	// AWB light area
    	{
		    // Strobe:FIXME
		    {
            0,    // i4RightBound
            0,    // i4LeftBound
            0,    // i4UpperBound
            0    // i4LowerBound
		    },
    		// Tungsten
    		{
            -189,    // i4RightBound
            -839,    // i4LeftBound
            -182,    // i4UpperBound
            -322    // i4LowerBound
    		},
    		// Warm fluorescent
    		{
            -189,    // i4RightBound
            -839,    // i4LeftBound
            -322,    // i4UpperBound
            -442    // i4LowerBound
    		},
    		// Fluorescent
    		{
            -61,    // i4RightBound
            -189,    // i4LeftBound
            -202,    // i4UpperBound
            -339    // i4LowerBound
            },
            // CWF
            {
            -61,    // i4RightBound
            -189,    // i4LeftBound
            -339,    // i4UpperBound
            -439    // i4LowerBound
            },
            // Daylight
            {
            145,    // i4RightBound
            -61,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
    		},
    		// Shade
    		{
            505,    // i4RightBound
            145,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
		},
		// Daylight Fluorescent
		{
            110,    // i4RightBound
            -61,    // i4LeftBound
            -372,    // i4UpperBound
            -439    // i4LowerBound
    		}
    	},
    	// PWB light area
    	{
    		// Reference area
    		{
            505,    // i4RightBound
            -839,    // i4LeftBound
            0,    // i4UpperBound
            -442    // i4LowerBound
    		},
    		// Daylight
    		{
            170,    // i4RightBound
            -61,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
    		},
    		// Cloudy daylight
    		{
            270,    // i4RightBound
            95,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
    		},
    		// Shade
    		{
            370,    // i4RightBound
            95,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
    		},
    		// Twilight
    		{
            -61,    // i4RightBound
            -221,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
    		},
    		// Fluorescent
    		{
            170,    // i4RightBound
            -239,    // i4LeftBound
            -232,    // i4UpperBound
            -439    // i4LowerBound
    		},
    		// Warm fluorescent
    		{
            -203,    // i4RightBound
            -403,    // i4LeftBound
            -232,    // i4UpperBound
            -439    // i4LowerBound
            },
            // Incandescent
            {
            -203,    // i4RightBound
            -403,    // i4LeftBound
            -192,    // i4UpperBound
            -372    // i4LowerBound
            },
            // Gray World
            {
            5000,    // i4RightBound
            -5000,    // i4LeftBound
            5000,    // i4UpperBound
            -5000    // i4LowerBound
		    }
    	},
    	// PWB default gain
    	{
    		// Daylight
    		{
            838,    // i4R
    		512,	// i4G
            651    // i4B
    		},
    		// Cloudy daylight
    		{
            971,    // i4R
    		512,	// i4G
            536    // i4B
    		},
    		// Shade
    		{
            1029,    // i4R
    			512,	// i4G
            496    // i4B
    		},
    		// Twilight
    		{
            670,    // i4R
    			512,	// i4G
            878    // i4B
    		},
    		// Fluorescent
    		{
            821,    // i4R
    			512,	// i4G
            793    // i4B
            },
            // Warm fluorescent
            {
            604,    // i4R
            512,    // i4G
            1195    // i4B
            },
            // Incandescent
            {
            556,    // i4R
            512,    // i4G
            1124    // i4B
            },
            // Gray World
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            }
        },
        // AWB preference color	
        {
            // Tungsten
            {
            0,    // i4SliderValue
            6859    // i4OffsetThr
    		},
    		// Warm fluorescent
    		{
            0,    // i4SliderValue
            5247    // i4OffsetThr
    		},
    		// Shade
    		{
            0,    // i4SliderValue
            1344    // i4OffsetThr
    		},
    		// Daylight WB gain
    		{
            752,    // i4R
            512,    // i4G
            520    // i4B
		},
		// Preference gain: strobe
		{
			512,	// i4R
			512,	// i4G
			512	// i4B
		},
		// Preference gain: tungsten
		{
            460,    // i4R
			512,	// i4G
            540    // i4B
		},
		// Preference gain: warm fluorescent
		{
            460,    // i4R
			512,	// i4G
            540    // i4B
		},
		// Preference gain: fluorescent
		{
			504,	// i4R //500
			512,	// i4G
			516	    // i4B  //520
		},
		// Preference gain: CWF
		{
			504,	// i4R  //
			512,	// i4G
            516    // i4B  //
		},
		// Preference gain: daylight
		{
            500,    // i4R
			512,	// i4G
            520    // i4B
		},
		// Preference gain: shade
		{
            500,    // i4R
			512,	// i4G
            520    // i4B
		},
		// Preference gain: daylight fluorescent
		{
            500,    // i4R
			512,	// i4G
            516    // i4B
    		}
    	},
        {// CCT estimation
            {// CCT
			    2300,	// i4CCT[0]
    			2850,	// i4CCT[1]
    			4100,	// i4CCT[2]
    			5100,	// i4CCT[3]
    			6500	// i4CCT[4]
    		},
            {// Rotated X coordinate
                -555,    // i4RotatedXCoordinate[0]
                -423,    // i4RotatedXCoordinate[1]
                -259,    // i4RotatedXCoordinate[2]
                -131,    // i4RotatedXCoordinate[3]
    			0	// i4RotatedXCoordinate[4]
    		}
    	}
    },
	{0}
};

#include INCLUDE_FILENAME_ISP_LSC_PARAM
//};  //  namespace


typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;


namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, VOID*const pDataBuf, UINT32 const size) const
{
    UINT32 dataSize[CAMERA_DATA_TYPE_NUM] = {sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT),
                                             sizeof(NVRAM_CAMERA_3A_STRUCT),
                                             sizeof(NVRAM_CAMERA_SHADING_STRUCT),
                                             sizeof(NVRAM_LENS_PARA_STRUCT),
                                             sizeof(AE_PLINETABLE_T)};

    if (CameraDataType > CAMERA_DATA_AE_PLINETABLE || NULL == pDataBuf || (size < dataSize[CameraDataType]))
    {
        return 1;
    }

    switch(CameraDataType)
    {
        case CAMERA_NVRAM_DATA_ISP:
            memcpy(pDataBuf,&CAMERA_ISP_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_3A:
            memcpy(pDataBuf,&CAMERA_3A_NVRAM_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_3A_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_SHADING:
            memcpy(pDataBuf,&CAMERA_SHADING_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_SHADING_STRUCT));
            break;
        case CAMERA_DATA_AE_PLINETABLE:
            memcpy(pDataBuf,&g_PlineTableMapping,sizeof(AE_PLINETABLE_T));
            break;
        default:
            break;
    }
    return 0;
}};  //  NSFeature


