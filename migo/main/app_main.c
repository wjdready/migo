/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "display.h"
#include "lvgl.h"
#include "lv_port_indev.h"
#include "lv_examples.h"


uint16_t frame[240*240*2];


static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Toggled\n");
    }
}

void lv_ex_btn_1(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn2, event_handler);
    lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_btn_set_checkable(btn2, true);
    lv_btn_toggle(btn2);
    lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Toggled");
}


static void lv_tick_task(void *arg)
{
  while (1)
  {
    lv_tick_inc(5);
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  printf("Hello LVGL!\n");

  lv_init();
  display_init();
  lv_port_indev_init();
  
  // Start background polling
  xTaskCreatePinnedToCore(&lv_tick_task, "lv_tick_task", 1024 * 2, NULL, 5, NULL, 1);

  lv_demo_keypad_encoder();
  // lv_ex_btn_1();

  while (1)
  {
    lv_task_handler();
    // printf("Hello World\n");
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
  
}
