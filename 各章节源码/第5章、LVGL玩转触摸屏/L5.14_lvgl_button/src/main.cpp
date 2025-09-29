#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336.h"
#include "RGB.h"

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
void my_touchpad_read(lv_indev_t* indev, lv_indev_data_t* data)
{
    ft.read();
    if(!ft.isTouched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = ft.points[0].x;
        data->point.y = ft.points[0].y;
    }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}

static void event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e); // 获取事件类型
    lv_obj_t* button = static_cast<lv_obj_t*>(lv_event_get_target(e));
    switch (code)
    {
    case LV_EVENT_CLICKED:
        {
            lv_obj_t* label = lv_obj_get_child(button, 0);
            char* label_text = lv_label_get_text(label);
            if (lv_strcmp(label_text, LV_SYMBOL_POWER" ON") == 0)
            {
                lv_label_set_text(label, LV_SYMBOL_POWER" OFF");
                lv_obj_set_style_bg_color(button, lv_color_hex(0xFF0000), 0);
                RGB::powerOn();
            }
            else
            {
                lv_label_set_text(label, LV_SYMBOL_POWER" ON");
                lv_obj_set_style_bg_color(button, lv_color_hex(0x00AA00), 0);
                RGB::powerOff();
            }
            break;
        }
    default:
        break;
    }
}

void setup()
{
    lv_init();
    ft.begin();
    RGB::begin();
    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);


    lv_display_t* disp;
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

    lv_indev_t* indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    // 创建一个对象
    lv_obj_t* button = lv_button_create(lv_scr_act());
    lv_obj_set_size(button, 100, 30);
    lv_obj_center(button);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x00AA00), 0);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_center(label);
    lv_label_set_text(label, LV_SYMBOL_POWER" ON");

    lv_obj_add_event_cb(button, event_handler, LV_EVENT_CLICKED, NULL);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}
