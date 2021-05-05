#ifndef _ST7789_H
#define _ST7789_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
#define ST7789_HOR_RES 240
#define ST7789_VER_RES 240

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void st7789_init();
int st7789_is_backlight_initialized();
void st7789_backlight_percentage_set(int value);
void st7789_backlight_deinit();
void st7789_prepare();
void st7789_poweroff();

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif