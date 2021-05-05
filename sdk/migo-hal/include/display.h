#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <stdint.h>
#include "disp_spi.h"
#include "st7789.h"

#define LCD_WIDTH ST7789_HOR_RES
#define LCD_HEIGHT ST7789_VER_RES

#ifdef __cplusplus
extern "C"
{
#endif

    void display_init();
    void backlight_deinit();
    void write_frame_rectangleLE(short left, short top, short width, short height, uint16_t *buffer);
    void display_clear(uint16_t color);
    void display_prepare();
    void display_poweroff();

#ifdef __cplusplus
}
#endif

#endif /*_DISPLAY_H_*/
