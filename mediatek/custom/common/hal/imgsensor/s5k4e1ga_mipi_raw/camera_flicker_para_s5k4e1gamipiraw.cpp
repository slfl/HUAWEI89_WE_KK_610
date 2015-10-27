#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_info_s5k4e1gamipiraw.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_flicker_para.h"
#include <cutils/xlog.h>


static void get_flicker_para_by_preview(FLICKER_CUST_PARA* para)
{
  int i;
  int freq[9] = {70,100, 120, 140, 170, 200, 220, 240, 260};
  FLICKER_CUST_STATISTICS EV50_L50 = {-229, 9985, 441, -948};
  FLICKER_CUST_STATISTICS EV50_L60 = {1236, 253, 1369, -472};
  FLICKER_CUST_STATISTICS EV60_L50 = {1251, 292, 1737, -530};
  FLICKER_CUST_STATISTICS EV60_L60 = {-219, 7278, 398, -870};
  for(i=0;i<9;i++)
  {
    para->flickerFreq[i]=freq[i];
  }
  para->flickerGradThreshold=42;
  para->flickerSearchRange=32;
  para->minPastFrames=3;
  para->maxPastFrames=14;
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  para->EV50_thresholds[0]=-30;
  para->EV50_thresholds[1]=8;
  para->EV60_thresholds[0]=-30;
  para->EV60_thresholds[1]=8;
  para->freq_feature_index[0]=2;
  para->freq_feature_index[1]=1;
}




typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;
namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetFlickerPara(MINT32 sensorMode, MVOID*const pDataBuf) const
{
	return 0;
}
}

