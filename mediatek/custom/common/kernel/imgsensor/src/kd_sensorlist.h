//s_add new sensor driver here
//export funtions

UINT32 BF3905MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 MT9T113_MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 MT9V113_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 OV5647MIPISensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 S5K4E1GA_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);

//! Add Sensor Init function here
//! Note:
//! 1. Add by the resolution from ""large to small"", due to large sensor
//!    will be possible to be main sensor.
//!    This can avoid I2C error during searching sensor.
//! 2. This file should be the same as mediatek\custom\common\hal\imgsensor\src\sensorlist.cpp
ACDK_KD_SENSOR_INIT_FUNCTION_STRUCT kdSensorList[MAX_NUM_OF_SUPPORT_SENSOR+1] =
{
#if defined(BF3905_MIPI_YUV)
    {BF3905MIPI_SENSOR_ID, SENSOR_DRVNAME_BF3905_MIPI_YUV, BF3905MIPI_YUV_SensorInit},
#endif
#if defined(MT9T113_MIPI_YUV)
    {MT9T113MIPI_SENSOR_ID, SENSOR_DRVNAME_MT9T113_MIPI_YUV,MT9T113MIPI_YUV_SensorInit},
#endif
#if defined(MT9V113_YUV)
    {MT9V113_SENSOR_ID, SENSOR_DRVNAME_MT9V113_YUV, MT9V113_YUV_SensorInit},
#endif
#if defined(OV5647_MIPI_RAW)
    {OV5647MIPI_SENSOR_ID, SENSOR_DRVNAME_OV5647MIPI_RAW, OV5647MIPISensorInit}, 
#endif
#if defined(S5K4E1GA_MIPI_RAW)
    {S5K4E1GA_SENSOR_ID, SENSOR_DRVNAME_S5K4E1GA_MIPI_RAW, S5K4E1GA_MIPI_RAW_SensorInit}, 
#endif

/*  ADD sensor driver before this line */
    {0,{0},NULL}, //end of list
};
//e_add new sensor driver here

