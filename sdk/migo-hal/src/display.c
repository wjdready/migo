#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "display.h"
#include "lv_port_disp.h"

#define LINE_BUFFERS (2)
#define LINE_COUNT (8)

uint16_t *line[LINE_BUFFERS];
extern uint16_t myPalette[];

void set_display_brightness(int percent)
{
  st7789_backlight_percentage_set(percent);
}

void backlight_deinit()
{
  st7789_backlight_deinit();
}

void display_prepare(int percent)
{
  st7789_prepare(percent);
}

void display_poweroff(int percent)
{
  st7789_poweroff();
}

void write_frame_rectangleLE(short left, short top, short width, short height, uint16_t *buffer)
{
    short x, y, xv, yv;
    int sending_line = -1;
    int calc_line = 0;
    if (left < 0)
        left = 0;
    if (top < 0)
        top = 0;
    if (width < 1)
        width = 1;
    if (height < 1)
        height = 1;
    if (buffer == NULL)
    {
        for (y = top; y < height + top; y++)
        {
            xv = 0;
            for (x = left; x < width + left; x++)
            {
                line[calc_line][xv] = 0;
                xv++;
            }

            if (sending_line != -1)
                send_line_finish();
            sending_line = calc_line;
            calc_line = (calc_line == 1) ? 0 : 1;
            send_lines_ext(y, left, width, line[sending_line], 1);
        }

        send_line_finish();
    }
    else
    {
        yv = 0;
        for (y = top; y < top + height; y++)
        {
            xv = 0;
            for (int i = left; i < left + width; ++i)
            {
                uint16_t pixel = buffer[yv * width + xv];
                line[calc_line][xv] = ((pixel << 8) | (pixel >> 8));
                xv++;
            }

            if (sending_line != -1)
                send_line_finish();
            sending_line = calc_line;
            calc_line = (calc_line == 1) ? 0 : 1;
            send_lines_ext(y, left, width, line[sending_line], 1);
            yv++;
        }
    }

    send_line_finish();
}


void display_clear(uint16_t color)
{
    int sending_line = -1;
    int calc_line = 0;
    // clear the buffer
    for (int i = 0; i < LINE_BUFFERS; ++i)
    {
        for (int j = 0; j < LCD_WIDTH * LINE_COUNT; ++j)
        {
            line[i][j] = color;
        }
    }

    for (int y = 0; y < LCD_HEIGHT; y += LINE_COUNT)
    {
        if (sending_line != -1)
            send_line_finish();
        sending_line = calc_line;
        calc_line = (calc_line == 1) ? 0 : 1;
        send_lines_ext(y, 0, LCD_WIDTH, line[sending_line], LINE_COUNT);
    }

    send_line_finish();
}

void display_init()
{
    // Line buffers
    const size_t lineSize = LCD_WIDTH * LINE_COUNT * sizeof(uint16_t);
    for (int x = 0; x < LINE_BUFFERS; x++)
    {
        line[x] = heap_caps_malloc(lineSize, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
        if (!line[x])
            abort();
    }
    // Initialize the LCD
    disp_spi_init();
    
    st7789_init();

    printf("start init lv disp\n");
    lv_port_disp_init();
}
