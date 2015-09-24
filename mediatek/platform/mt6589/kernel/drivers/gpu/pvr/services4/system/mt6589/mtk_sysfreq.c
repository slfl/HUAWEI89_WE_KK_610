
#include "mtk_sysfreq.h"
#include "mach/mt_gpufreq.h"
#include "mach/mt_clkmgr.h"
#include <mach/sync_write.h>

static bool mt_keep_freq_non_od_set = false;

#define MTK_GPU_DVFS 1

#if MTK_GPU_DVFS
static struct mt_gpufreq_info freqs_special_vrf18_2[] = {
    {GPU_DVFS_F3, 40, 100, GPU_POWER_VRF18_1_15V, 100},
    {GPU_DVFS_F5, 0,  40, GPU_POWER_VRF18_1_05V,  80},
};
static struct mt_gpufreq_info freqs_special2_vrf18_2[] = {
    {GPU_DVFS_F2, 60, 100, GPU_POWER_VRF18_1_15V, 100},
    {GPU_DVFS_F3, 30,  60, GPU_POWER_VRF18_1_15V,  90},
    {GPU_DVFS_F5, 0,  30, GPU_POWER_VRF18_1_05V,  75},
};
static struct mt_gpufreq_info freqs_special3_vrf18_2[] = {
    {GPU_DVFS_F1, 60, 100, GPU_POWER_VRF18_1_15V, 100},
    {GPU_DVFS_F3, 30,  60, GPU_POWER_VRF18_1_15V,  90},
    {GPU_DVFS_F5, 0,  30, GPU_POWER_VRF18_1_05V,  75},
};
#endif

void MtkInitSetFreqTbl(unsigned int tbltype)
{
    switch (tbltype)
    {
#if MTK_GPU_DVFS
    case TBLTYPE1:
//        printk("[GPU DVFS] register vrf18_2 special table ...\n");
        mt_gpufreq_register(freqs_special_vrf18_2, 2);
        break;
    case TBLTYPE2:
//        printk("[GPU DVFS] register vrf18_2 special2 table ...\n");
        mt_gpufreq_register(freqs_special2_vrf18_2, 3);
        break;
    case TBLTYPE3:
//        printk("[GPU DVFS] register vrf18_2 special3 table ...\n");
        mt_gpufreq_register(freqs_special3_vrf18_2, 3);
        break;
#endif
    default:
    case TBLTYPE0:
        mt_gpufreq_non_register();
        break;
    }
}

/* 	Table for overclock GPU
	GPU_DVFS_F1 (476666)
	GPU_DVFS_F2 (403000)
	GPU_DVFS_F3 (357500)
	GPU_DVFS_F4 (312000)
	GPU_DVFS_F5 (286000)
	GPU_DVFS_F6 (268666)
	GPU_DVFS_F7 (238333)
	GPU_DVFS_F8 (156000)
*/
PVRSRV_ERROR MTKSetFreqInfo(unsigned int freq, unsigned int tbltype)
{
    printk(" freq= %d", freq);

	freq = GPU_DVFS_F3;
	tbltype = TBLTYPE1;

#if defined(MTK_FREQ_OD_INIT)
    if (freq > GPU_DVFS_F5)
    {
		mt_gpufreq_set_initial(freq, GPU_POWER_VRF18_1_05V);
		mt65xx_reg_sync_writel((readl(CLK_CFG_8)&0xffcffff)|0x30000, CLK_CFG_8);
	}
	else
#endif
    {
		mt_gpufreq_set_initial(freq, GPU_POWER_VRF18_1_05V);
    }
		mt_gpufreq_keep_frequency_non_OD_init(GPU_KEEP_FREQ_NON_OD_BYPASS, GPU_KEEP_VOLT_NON_OD_BYPASS);

    MtkInitSetFreqTbl(tbltype);

    return PVRSRV_OK;
}

void MtkSetKeepFreq(void)
{
    if (mt_gpufreq_keep_frequency_non_OD_get())
    {
        if (mt_keep_freq_non_od_set==false)
        {
            mt_gpufreq_keep_frequency_non_OD_set(1);
            mt_keep_freq_non_od_set=true;
        }
    }
    else
    {
        if (mt_keep_freq_non_od_set==true)
        {
            mt_gpufreq_keep_frequency_non_OD_set(0);
            mt_keep_freq_non_od_set=false;
        }
    }
}

