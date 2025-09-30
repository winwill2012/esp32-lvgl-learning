#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336.h"
#include "fl/mutex.h"
#include "fl/task.h"

#define TOUCH_SDA 10
#define TOUCH_SCL 13
#define TOUCH_INT 12
#define TOUCH_RST 9
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

FT6336 ft(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TFT_HOR_RES, TFT_VER_RES);

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    ft.read();
    if (!ft.isTouched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = ft.points[0].x;
        data->point.y = ft.points[0].y;
    }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void) {
    return millis();
}

static lv_obj_t *label;
int count = 0;

/**
// 以下是通过互斥锁的方式来保证线程安全的：

QueueHandle_t mutex;

void update_label_text_color(lv_color_t color) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
        lv_obj_set_style_text_color(label, color, 0);
        xSemaphoreGive(mutex);
    }
}

void add(void *ptr) {
    while (true) {
        count++;
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            lv_label_set_text_fmt(label, "%d", count);
            update_label_text_color(lv_color_hex(0x00FF00));
            lv_obj_center(label);
            xSemaphoreGive(mutex);
        }
        vTaskDelay(25);
    }
}

void minus(void *ptr) {
    while (true) {
        count--;
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            lv_label_set_text_fmt(label, "%d", count);
            update_label_text_color(lv_color_hex(0xFF0000));
            lv_obj_center(label);
            xSemaphoreGive(mutex);
        }
        vTaskDelay(50);
    }
}
*/

// 以下是通过定时器的方式来保证线程安全的
void update_label_text_color(lv_color_t color) {
    lv_obj_set_style_text_color(label, color, 0);
}

void add(lv_timer_t *timer) {
    count++;
    lv_label_set_text_fmt(label, "%d", count);
    update_label_text_color(lv_color_hex(0x00FF00));
    lv_obj_center(label);
}

void minus(lv_timer_t *timer) {
    count--;
    lv_label_set_text_fmt(label, "%d", count);
    update_label_text_color(lv_color_hex(0xFF0000));
    lv_obj_center(label);
}

void setup() {
    Serial.begin(115200);
    lv_init();
    ft.begin();
    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    lv_display_t *disp;
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(label, "%d", 0);
    lv_obj_center(label);

    // xTaskCreate(add, "add", 2048, nullptr, 1, nullptr);
    // xTaskCreate(minus, "minus", 2048, nullptr, 1, nullptr);

    lv_timer_create(add, 25, nullptr);
    lv_timer_create(minus, 50, nullptr);
}

void loop() {
    // if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
    lv_timer_handler(); /* let the GUI do its work */
    // xSemaphoreGive(mutex);
    // }
    delay(5); /* let this time pass */
}
