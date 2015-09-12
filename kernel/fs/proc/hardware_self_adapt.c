
#include <linux/hardware_self_adapt.h>
#include <linux/kernel.h>
#include <linux/module.h>


/******************************************************************************
  Function:       get_hardware_product_version
  Description:    get the hardware_product_version
  Input:          void
  Output:         None
  Return:         None
  Others:         None
******************************************************************************/
hw_product_type get_hardware_product_version( void )
{
    return hw_product_info.board_id;
}


/******************************************************************************
  Function:       get_hardware_battery_type
  Description:    Get battery id
  Input:          null
  Output:         null
  Return:         battery id
  Others:         null
******************************************************************************/
hw_battery_type get_hardware_battery_type( void )
{
    return hw_product_info.battery_id;
}
#ifdef CONFIG_DL_CHECK_SUPPORT
hw_dl_check_tag get_hardware_dl_check_tag( void )
{
    return hw_product_info.dl_check_tag;
}
#endif
