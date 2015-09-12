
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
    #include <platform/disp_drv_platform.h>
#elif defined(BUILD_UBOOT)
    #include <asm/arch/mt_gpio.h>
#else
    #include <linux/delay.h>
    #include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  		(540)
#define FRAME_HEIGHT 		(960)

#define REGFLAG_DELAY       		0XFE
#define REGFLAG_END_OF_TABLE    	0xFD   // END OF REGISTERS MARKER 
#define LCM_ID	(0x80)
//#define LCD_ID_P0 GPIO16
//#define LCD_ID_P1 GPIO104
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif


const static unsigned char LCD_MODULE_ID = 0x02;

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    			(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 				(lcm_util.udelay(n))
#define MDELAY(n) 				(lcm_util.mdelay(n))
#define LCM_DSI_CMD_MODE      1
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
//#define read_reg                                            lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)                   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
const static unsigned int BL_MIN_LEVEL = 20;
struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[128];
};
/*modify the chimei LCD setting parameter*/
static struct LCM_setting_table truly_ips_init[] = {

    /*
    Note :

    Data ID will depends on the following rule.

    count of parameters > 1	=> Data ID = 0x39
    count of parameters = 1	=> Data ID = 0x15
    count of parameters = 0	=> Data ID = 0x05

    Structure Format :

    {DCS command, count of parameters, {parameter list}}
    {REGFLAG_DELAY, milliseconds of time, {}},

    ...

    Setting ending by predefined flag

    {REGFLAG_END_OF_TABLE, 0x00, {}}
    */
    {0xFF,5,{0xAA,0x55,0x25,0x01,0x01}},


    {0xF2,35,{0x00,0x00,0x4A,0x0A,0xA8,
                   0x00,0x00,0x00,0x00,0x00,
                   0x00,0x00,0x00,0x00,0x00,
                   0x00,0x00,0x0B,0x00,0x00,
                   0x00,0x00,0x00,0x00,0x00,
                   0x00,0x00,0x00,0x40,0x01,
                   0x51,0x00,0x01,0x00,0x01}},

    {0xF3,7,{0x02,0x03,0x07,0x45,0x88,0xD1,0x0D}},

    // Set page 0
    {0xF0,5,{0x55,0xaa,0x52,0x08,0x00}},
    {0xB1,3,{0xfc,0x00,0x00}},
    {0xBC,3,{0x00,0x00,0x00}}, //00 colume inversion;02 2dot inversion, 01 1dot inversion
    {0xB8,4,{0x01,0x02,0x02,0x02}}, //EQ Control Function for Sourc
    {0xC9,6,{0x63,0x06,0x0D,0x1A,0x17,0x00}},

    {0xF0,5,{0x55,0xaa,0x52,0x08,0x00}},
    {0xB1,3,{0xfc,0x00,0x00}},
    {0xBB,4,{0x01,0x02,0x02,0x02}},
    {0xC9,6,{0x63,0x06,0x0D,0x1A,0x17,0x00}},

    // Set page 1
    {0xF0,5,{0x55,0xaa,0x52,0x08,0x01}},
    {0xB0,3,{0x05,0x05,0x05}},
    {0xB1,3,{0x05,0x05,0x05}},
    {0xB2,3,{0x01,0x01,0x01}},
    {0xB3,3,{0x0D,0x0D,0x0D}},
    {0xB4,3,{0x09,0x09,0x09}},
    {0xB6,3,{0x44,0x44,0x44}},
    {0xB7,3,{0x34,0x34,0x34}},
    {0xB8,3,{0x10,0x10,0x10}},
    {0xB9,3,{0x26,0x26,0x26}},
    {0xBA,3,{0x34,0x34,0x34}},
    {0xBC,3,{0x00,0xB5,0x00}},
    {0xBD,3,{0x00,0xD2,0x00}},
    //{0xBE,1,{0x58}},//setting vcom
    {0xC0,2,{0x04,0x00}},
    {0xCA,1,{0x00}},

    
    {0xD0,4,{0x06,0x10,0x0D,0x0F}},
    // Gamma for R+
    {0xD1,16,{0x00,0xA7,0x01,0x0F,
                     0x01,0x34,0x01,0x49,
                     0x01,0x56,0x01,0x75,
                     0x01,0x95,0x01,0xB7}},
    // Gamma for R+
    {0xD2,16,{0x01,0xDA,0x02,0x03,
                     0x02,0x23,0x02,0x57,
                     0x02,0x84,0x02,0x85,
                     0x02,0xA9,0x02,0xDD}},

    // Gamma for R+
    {0xD3,16,{0x02,0xF8,0x03,0x2D,
                     0x03,0x52,0x03,0x6D,
                     0x03,0x83,0x03,0x9C,
                     0x03,0xAA,0x03,0xB6}},
    // Gamma for R+
    {0xD4,4,{0x03,0xC5,0x03,0xCF}},
    // Gamma for G+
    {0xD5,16,{0x00,0xA7,0x01,0x0F,
                     0x01,0x34,0x01,0x49,
                     0x01,0x56,0x01,0x75,
                     0x01,0x95,0x01,0xB7}},
    // Gamma for G+
    {0xD6,16,{0x01,0xDA,0x02,0x03,
                     0x02,0x23,0x02,0x57,
                     0x02,0x84,0x02,0x85,
                     0x02,0xA9,0x02,0xDD}},
    // Gamma for G+
    {0xD7,16,{0x02,0xF8,0x03,0x2D,
                     0x03,0x52,0x03,0x6D,
                     0x03,0x83,0x03,0x9C,
                     0x03,0xAA,0x03,0xB6}},
    // Gamma for G+
    {0xD8,4,{0x03,0xC5,0x03,0xCF}},
   // Gamma for B+
    {0xD9,16,{0x00,0xA7,0x01,0x0F,
                     0x01,0x34,0x01,0x49,
                     0x01,0x56,0x01,0x75,
                     0x01,0x95,0x01,0xB7}},
    // Gamma for B+
    {0xDD,16,{0x01,0xDA,0x02,0x03,
                     0x02,0x23,0x02,0x57,
                     0x02,0x84,0x02,0x85,
                     0x02,0xA9,0x02,0xDD}},
    // Gamma for B+
    {0xDE,16,{0x02,0xF8,0x03,0x2D,
                     0x03,0x52,0x03,0x6D,
                     0x03,0x83,0x03,0x9C,
                     0x03,0xAA,0x03,0xB6}},
    // Gamma for B+
    {0xDF,4,{0x03,0xC5,0x03,0xCF}},
    // Gamma for R
    {0xE0,16,{0x00,0xA7,0x01,0x0F,
                     0x01,0x34,0x01,0x49,
                     0x01,0x56,0x01,0x75,
                     0x01,0x95,0x01,0xB7}},
    // Gamma for R
    {0xE1,16,{0x01,0xDA,0x02,0x03,
                     0x02,0x23,0x02,0x57,
                     0x02,0x84,0x02,0x85,
                     0x02,0xA9,0x02,0xDD}},
    // Gamma for R
    {0xE2,16,{0x02,0xF8,0x03,0x2D,
                     0x03,0x52,0x03,0x6D,
                     0x03,0x83,0x03,0x9C,
                     0x03,0xAA,0x03,0xB6}},

    // Gamma for R
    {0xE3,4,{0x03,0xC5,0x03,0xCF}},

    // Gamma for G
    {0xE4,16,{0x00,0xA7,0x01,0x0F,
                     0x01,0x34,0x01,0x49,
                     0x01,0x56,0x01,0x75,
                     0x01,0x95,0x01,0xB7}},


    // Gamma for G
    {0xE5,16,{0x01,0xDA,0x02,0x03,
                     0x02,0x23,0x02,0x57,
                     0x02,0x84,0x02,0x85,
                     0x02,0xA9,0x02,0xDD}},

    // Gamma for G
    {0xE6,16,{0x02,0xF8,0x03,0x2D,
                     0x03,0x52,0x03,0x6D,
                     0x03,0x83,0x03,0x9C,
                     0x03,0xAA,0x03,0xB6}},

    // Gamma for G
    {0xE7,4,{0x03,0xC5,0x03,0xCF}},

    // Gamma for B
    {0xE8,16,{0x00,0xA7,0x01,0x0F,
                     0x01,0x34,0x01,0x49,
                     0x01,0x56,0x01,0x75,
                     0x01,0x95,0x01,0xB7}},


    // Gamma for B
    {0xE9,16,{0x01,0xDA,0x02,0x03,
                     0x02,0x23,0x02,0x57,
                     0x02,0x84,0x02,0x85,
                     0x02,0xA9,0x02,0xDD}},
    // Gamma for B
    {0xEA,16,{0x02,0xF8,0x03,0x2D,
                     0x03,0x52,0x03,0x6D,
                     0x03,0x83,0x03,0x9C,
                     0x03,0xAA,0x03,0xB6}},

    // Gamma for B
    {0xEB,4,{0x03,0xC5,0x03,0xCF}},     
    
    {0x35,1,{0x00}},//Open the TE function
    {0x11,1,{0x00}},
    {REGFLAG_DELAY, 120, {}},
    {0x29,1,{0x00}},
    {REGFLAG_DELAY, 20, {}},

    // Note
    // Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.
    // Setting ending by predefined flag
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
    {0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 0, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
    // Display off sequence
    {0x28, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Sleep Mode On
    {0x10, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	
	//{0x53, 1, {0x24}},
	//{0x55,1,{0x03}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


/*Optimization LCD initialization time*/
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                if(table[i].count <= 20)
                    mdelay(table[i].count);
                else
                    msleep(table[i].count);
                break;
            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
                //MDELAY(2);
        }
    }

}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
        memset(params, 0, sizeof(LCM_PARAMS));
    
        params->type   = LCM_TYPE_DSI;

        params->width  = FRAME_WIDTH;
        params->height = FRAME_HEIGHT;
#if (LCM_DSI_CMD_MODE)
        // enable tearing-free
        params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
        params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
#endif

#if (LCM_DSI_CMD_MODE)
        params->dsi.mode   = CMD_MODE;
#else
        params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
        // DSI
        /* Command mode setting */
        params->dsi.LANE_NUM                = LCM_TWO_LANE;
        //The following defined the fomat for data coming from LCD engine.
        params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
        params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
        params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
        params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;


        // Video mode setting       
        params->dsi.intermediat_buffer_num = 2;

        params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

        params->dsi.vertical_sync_active                = 3;
        params->dsi.vertical_backporch                  = 12;
        params->dsi.vertical_frontporch                 = 2;
        params->dsi.vertical_active_line                = FRAME_HEIGHT;

        params->dsi.horizontal_sync_active              = 10;
        params->dsi.horizontal_backporch                = 50;
        params->dsi.horizontal_frontporch               = 50;
        params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
        //refresh rate = 60fps , IC spec need clk < 275.5MHz
        params->dsi.PLL_CLOCK =LCM_DSI_6589_PLL_CLOCK_240_5;
}
static void lcm_init(void)
{
    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);  //huawei use GPIO 49: LSA0 to be reset pin
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    mdelay(30);//lcm power on , reset output high , delay 30ms ,then output low
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    msleep(30);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    msleep(50);

    push_table(truly_ips_init, sizeof(truly_ips_init) / sizeof(struct LCM_setting_table), 1);

}
static void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("LCD nt35516_truly lcm_suspend\n");
#else
	printk("LCD nt35516_truly lcm_suspend\n");
#endif
    push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("LCD nt35516_truly lcm_resume\n");
#else
	printk("LCD nt35516_truly lcm_resume\n");
#endif

	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

    unsigned int data_array[16];

    data_array[0]= 0x00053902;
    data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
    data_array[2]= (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);
    data_array[0]= 0x00053902;
    data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
    data_array[2]= (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    
    data_array[0]= 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);

}

/*heighten the brightness of qimei LCD*/
static void lcm_setbacklight(unsigned int level)
{
	// Refresh value of backlight level.
        unsigned int tmp_level = 0;
        //After improving LCD refresh rate , Power waste is high , so need reduce brightness
        tmp_level = level * 70 / 100;
        //lcm_backlight_level_setting[0].para_list[0] = level;
        lcm_backlight_level_setting[0].para_list[0] = tmp_level;

#ifdef BUILD_LK
        printf("LCD nt35516_truly lcm_setbacklight tmp_level=%d,level=%d\n",tmp_level,level);
#else
        printk("LCD nt35516_truly lcm_setbacklight tmp_level=%d,level=%d\n",tmp_level,level);
#endif

	push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}
/******************************************************************************
  Function:       lcm_set_pwm_level
  Description:    set different values for each LCD
  Input:          level
  Output:         NONE
  Return:         mapped_level
  Others:         none
******************************************************************************/
static unsigned int lcm_set_pwm_level(unsigned int level )
{
    unsigned int mapped_level = 0;
    if( 0 == level)
    {
        mapped_level = level;
    }
    else if(( 0 < level ) && (  BL_MIN_LEVEL > level ))
    {
        //Some 3rd APK will set values < 20 , set value(1-19) is 9
        mapped_level = 9;
    }
    else
    {
        //Reduce min brightness value
        mapped_level = (unsigned int)((level-8) * 8 /10);
    }
    #ifdef BUILD_LK
        printf("uboot:nt35516_lcm_set_pwm mapped_level = %d,level=%d\n",mapped_level,level);
    #else
        printk("kernel:nt35516_lcm_set_pwm mapped_level = %d,level=%d\n",mapped_level,level);
    #endif
    return mapped_level;
}

#ifndef BUILD_LK
//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
static unsigned int count = 0;
static unsigned int uncount = 0;
static unsigned int recount = 0;


static unsigned int lcm_esd_check(void)
{
    static int err_count = 0;
    unsigned char buffer_1[12];
    unsigned int array[16];
    int i;
    unsigned char fResult;

    //printk("lcm_esd_check<<<\n");
    for(i = 0; i < 12; i++)
      buffer_1[i] = 0x00;

    //---------------------------------
    // Set Maximum Return Size
    //---------------------------------
    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    //---------------------------------
    // Read [9Ch, 00h, ECC] + Error Report(4 Bytes)
    //---------------------------------
    read_reg_v2(0x0A, buffer_1, 7);

#if 0
    printk(KERN_EMERG "lcm_esd_check: read(0x0A)\n");
    for(i = 0; i < 7; i++)
      printk(KERN_EMERG "buffer_1[%d]:0x%x \n",i,buffer_1[i]);
#endif

    // printk(KERN_EMERG "jjyang lcm_esd_check read(0x0A)=%x\n",read_reg(0x0A));
    // printk("jjyang lcm_esd_check ID read(0x04)=%x,%x,%x\n",read_reg(0x04),read_reg(0x04),read_reg(0x04));
    // printk("jjyang lcm_esd_check ID read(0xDB)=%x\n",read_reg(0xDB));

    //---------------------------------
    // Judge Readout & Error Report
    //---------------------------------
    if(buffer_1[3] == 0x02) // Check data identifier of error report
    {
      if(buffer_1[4] & 0x02) // Check SOT sync error
        err_count++;
      else
        err_count = 0;
    }
    else
    {
      err_count = 0;
    }

    //printk(KERN_EMERG "jjyang lcm_esd_check err_count=%d\n",err_count);
    if((buffer_1[0] != 0x9C) || (err_count >= 2))
    {
      err_count = 0;
      uncount++;

      //printk(KERN_EMERG "jjyang lcm_esd_check unnormal uncount=%d\n",uncount);
      //printk("lcm_esd_check>>>\n");

      fResult = 1;
      //return TRUE;
    }
    else
    {
      count++;
      //printk(KERN_EMERG "jjyang lcm_esd_check normal count=%d\n",count);
      //printk("lcm_esd_check>>>\n");

      fResult = 0;
      //return FALSE;
    }

    //---------------------------------
    // Shut-Down Peripherial
    //---------------------------------
    array[0] = 0x00002200;
    dsi_set_cmdq(array, 1, 1);

    //---------------------------------
    // Set Maximum Return Size
    //---------------------------------
    array[0] = 0x00033700;
    dsi_set_cmdq(array, 1, 1);

    //---------------------------------
    // Clear D-PHY Buffer
    // Read [WC, WC, ECC, P1, P2, P3, CRC0, CRC1]+ Error Report(4 Bytes)
    //---------------------------------
    read_reg_v2(0xBC, buffer_1, 12);
#if 0
    printk(KERN_EMERG "lcm_esd_check: read(0xBC)\n");
    for(i = 0; i < 12; i++)
      printk(KERN_EMERG "buffer_1[%d]:0x%x \n",i,buffer_1[i]);
#endif

    if(fResult) return TRUE;
    else return FALSE;
}

/*heighten the brightness of qimei LCD*/
static unsigned int lcm_esd_recover(void)
{

    unsigned char para = 0;

	//printk(KERN_EMERG "jjyang lcm_esd_recover\n");

    lcm_init();


    //MDELAY(10);
    //push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    //MDELAY(10);

        /*heighten the brightness of qimei LCD*/
	lcm_setbacklight(200);

	recount++;

	printk(KERN_EMERG "jjyang lcm_esd_recover recover recount=%d\n",recount);


    return TRUE;
}
#endif
static unsigned int lcm_compare_id(void)
{
    unsigned char LCD_ID_value = 0;
#if 0
    unsigned int array[16];
    unsigned char buffer[12];
    unsigned int id = 0;
    int i;

    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);  //huawei use GPIO 49: LSA0 to be reset pin
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    /*Optimization LCD initialization time*/
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(30);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);

    for(i = 0; i < 12; i++)
        buffer[i] = 0x00;


    array[0] = 0x00033700;
    dsi_set_cmdq(array, 1, 1);

    // Read [WC, WC, ECC, P1, P2, P3, CRC0, CRC1]+ Error Report(4 Bytes)
    read_reg_v2(0x04, buffer, 12);

/*
#ifdef BUILD_UBOOT
    for(i = 0; i < 12; i++)
      printf("buffer[%d]:0x%x \n",i,buffer[i]);
#else
    for(i = 0; i < 12; i++)
	  printk(KERN_EMERG "buffer[%d]:0x%x \n",i,buffer[i]);

#endif
*/
    id = buffer[1];

    return ( LCM_ID == id ? 1 :0);
#endif
    #ifdef BUILD_LK
	printf("nt35516_lcm_compare_id\n");
#else
	printk("nt35516_lcm_compare_id\n");
#endif
#if 0
    LCD_ID_value = which_lcd_module();
    if(LCD_MODULE_ID == LCD_ID_value)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#endif
}
LCM_DRIVER truly_nt35516_lcm_drv =
{
    .name           = "truly_nt35516",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
    /*heighten the brightness of qimei LCD*/
    .set_backlight  = lcm_setbacklight,
    .set_pwm_level = lcm_set_pwm_level,
    //.set_pwm      = lcm_setpwm,
    //.get_pwm      = lcm_getpwm
//    .set_cabcmode = lcm_setcabcmode,
//    .esd_check     = lcm_esd_check,
    /*heighten the brightness of qimei LCD*/
//    .esd_recover       = lcm_esd_recover,        
    .compare_id     = lcm_compare_id,
#endif
};
