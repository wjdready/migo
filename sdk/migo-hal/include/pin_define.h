/*
 * Author       : weijundong
 * LastEditors  : weijundong
 * Date         : 2021-05-02 10:58:09
 * LastEditTime : 2021-05-05 09:08:21
 * FilePath     : /esplay/home/shino/esp/Projects/migo/sdk/migo-hal/include/pin_define.h
 * Description  : 
 */
#ifndef __PIN_DEFINE_H__
#define __PIN_DEFINE_H__

#include <driver/adc.h>

/* TFT */
#define DISP_SPI_MOSI 13
#define DISP_SPI_CLK 12
#define DISP_SPI_CS 27
#define DISP_SPI_DC 26
#define DISP_BCKL 25

/* KEYPAD */
#define MENU    35
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDR 0x20

/* SD CARD */
#define SD_PIN_NUM_MISO 23
#define SD_PIN_NUM_MOSI 4
#define SD_PIN_NUM_CLK  18
#define SD_PIN_NUM_CS 5

/* AUDIO */
#define I2S_BCK 15
#define I2S_WS 2
#define I2S_DOUT 14
#define I2S_DIN 19

/* POWER */
#define USB_PLUG_PIN 32
#define CHRG_STATE_PIN 36
#define ADC_PIN ADC1_CHANNEL_3

#endif
