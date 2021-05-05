#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "st7789.h"
#include "disp_spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/ledc.h"
#include "driver/rtc_io.h"
#include "pin_define.h"

#define TFT_CMD_SWRESET 0x01
#define TFT_CMD_SLEEP 0x10
#define TFT_CMD_DISPLAY_OFF 0x28

/**********************
 *  STATIC VARIABLES
 **********************/
static const int DUTY_MAX = 0x1fff;
static const int LCD_BACKLIGHT_ON_VALUE = 1;
static bool is_bklight_init = false;


typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;


DRAM_ATTR static const lcd_init_cmd_t st7789_sleep_cmds[] = {
    {TFT_CMD_SWRESET, {0}, 0x80},
    {TFT_CMD_DISPLAY_OFF, {0}, 0x80},
    {TFT_CMD_SLEEP, {0}, 0x80},
    {0, {0}, 0xff}
};

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void st7789_send_cmd(const uint8_t cmd)
{
    disp_spi_send(&cmd, 1, CMD_ON);
}

static void st7789_send_data(const void *data, uint16_t length)
{
    disp_spi_send(data, length, DATA_ON);
}

void st7789_backlight_deinit()
{
    ledc_fade_func_uninstall();
    esp_err_t err = ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    if (err != ESP_OK)
    {
        printf("%s: ledc_stop failed.\n", __func__);
    }
}

static void backlight_init()
{
    //configure timer0
    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer));

    ledc_timer.duty_resolution = LEDC_TIMER_13_BIT; //set timer counter bit number
    ledc_timer.freq_hz = 5000;                      //set frequency of pwm
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;    //timer mode,
    ledc_timer.timer_num = LEDC_TIMER_0;            //timer index

    ledc_timer_config(&ledc_timer);

    //set the configuration
    ledc_channel_config_t ledc_channel;
    memset(&ledc_channel, 0, sizeof(ledc_channel));

    //set LEDC channel 0
    ledc_channel.channel = LEDC_CHANNEL_0;
    //set the duty for initialization.(duty range is 0 ~ ((2**duty_resolution)-1)
    ledc_channel.duty = (LCD_BACKLIGHT_ON_VALUE) ? 0 : DUTY_MAX;
    //GPIO number
    ledc_channel.gpio_num = DISP_BCKL;
    //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
    ledc_channel.intr_type = LEDC_INTR_FADE_END;
    //set LEDC mode, from ledc_mode_t
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    //set LEDC timer source, if different channel use one timer,
    //the frequency and duty_resolution of these channels should be the same
    ledc_channel.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel);

    //initialize fade service.
    ledc_fade_func_install(0);

    // duty range is 0 ~ ((2**duty_resolution)-1)
    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (LCD_BACKLIGHT_ON_VALUE) ? DUTY_MAX : 0, 500);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);

    is_bklight_init = true;

    printf("Backlight initialization done.\n");
}

int st7789_is_backlight_initialized()
{
    return is_bklight_init;
}

void st7789_backlight_percentage_set(int value)
{
    int duty = DUTY_MAX * (value * 0.01f);

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 500);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
}

void st7789_poweroff()
{
    esp_err_t err = ESP_OK;

    // fade off backlight
    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (LCD_BACKLIGHT_ON_VALUE) ? 0 : DUTY_MAX, 100);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);

    // Disable LCD panel
    int cmd = 0;
    while (st7789_sleep_cmds[cmd].databytes != 0xff)
    {
        st7789_send_cmd(st7789_sleep_cmds[cmd].cmd);
        st7789_send_data(st7789_sleep_cmds[cmd].data, st7789_sleep_cmds[cmd].databytes & 0x7f);
        if (st7789_sleep_cmds[cmd].databytes & 0x80)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        cmd++;
    }

    err = rtc_gpio_init(DISP_BCKL);
    if (err != ESP_OK)
    {
        abort();
    }

    err = rtc_gpio_set_direction(DISP_BCKL, RTC_GPIO_MODE_OUTPUT_ONLY);
    if (err != ESP_OK)
    {
        abort();
    }

    err = rtc_gpio_set_level(DISP_BCKL, LCD_BACKLIGHT_ON_VALUE ? 0 : 1);
    if (err != ESP_OK)
    {
        abort();
    }
}

void st7789_prepare()
{
    // Return use of backlight pin
    esp_err_t err = rtc_gpio_deinit(DISP_BCKL);
    if (err != ESP_OK)
    {
        abort();
    }
}

void st7789_init()
{
    lcd_init_cmd_t init_cmds[] = {
        {0x01, {0x00}, 0x80},                // Power Control 1
        {0x11, {0x00}, 0x80},                // Power Control 2
        {0x3A, {0x55}, 1},                   // VCOM Control 1
        {0x36, {0x60}, 1},                   // VCOM Control 2
        {0x2A, {0x00, 0x00, 0x00, 0xF0}, 4}, // Memory Access Control
        {0x2B, {0x00, 0x00, 0x00, 0xF0}, 4}, // Pixel Format Set
        {0x21, {0x00}, 0x80},                // Display Inversion OFF
        {0x13, {0x00}, 0x80},                // Frame Rate Control
        {0x29, {0x00}, 0x80},                // Display ON
        {0, {0}, 0xff}};

    //Initialize non-SPI GPIOs
    gpio_set_direction(DISP_BCKL, GPIO_MODE_OUTPUT);

    //Send all the commands
    uint16_t cmd = 0;
    while (init_cmds[cmd].databytes != 0xff)
    {
        st7789_send_cmd(init_cmds[cmd].cmd);
        st7789_send_data(init_cmds[cmd].data, init_cmds[cmd].databytes & 0x1F);
        if (init_cmds[cmd].databytes & 0x80)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        cmd++;
    }

    ///Enable backlight
    printf("Enable backlight.\n");
    backlight_init();
}
