/*
 * arch/arm/mach-meson/include/mach/i2c-aml.h
 */

 /* just a sample. you can add private i2c platform data*/
#ifndef  _MACH_AML_LCD_H
#define _MACH_AML_LCD_H

#include <plat/platform_data.h>
#include <linux/vout/lcdoutc.h>

struct aml_lcd_platform {
	plat_data_public_t  public ;
    struct Lcd_Config_t  *lcd_conf;
	/* local settings */
	int lcd_status;
};
#endif