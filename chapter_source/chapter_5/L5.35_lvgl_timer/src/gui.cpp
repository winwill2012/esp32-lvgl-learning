#include "gui.h"
#include "lvgl.h"
#include "WiFi.h"

static lv_obj_t* list;
static lv_obj_t* image;
static bool isScanning = false;
static int32_t angle = 0;

void timer_event_callback(lv_timer_t* timer)
{
    angle = (angle + 450) % 3600;
    lv_obj_set_style_transform_rotation(image, angle, 0);

    int16_t result = WiFi.scanComplete();
    if (result >= 0)
    {
        lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
        for (int i = 0; i < result; i++)
        {
            lv_list_add_button(list, LV_SYMBOL_WIFI, WiFi.SSID(i).c_str());
        }
        lv_timer_delete(timer);
        isScanning = false;
        angle = 0;
    }
}

void button_event_callback(lv_event_t* e)
{
    auto code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (!isScanning)
        {
            auto child = lv_obj_get_child(list, 0);
            while (child != nullptr)
            {
                lv_obj_delete(child);
                child = lv_obj_get_child(list, 0);
            }
            lv_obj_remove_flag(image, LV_OBJ_FLAG_HIDDEN);
            WiFi.scanNetworks(true);
            lv_timer_create(timer_event_callback, 500, nullptr);
            isScanning = true;
        }
    }
}

void show_ui()
{
    list = lv_list_create(lv_scr_act());
    lv_obj_set_size(list, lv_pct(95), lv_pct(85));
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -5);

    auto button = lv_button_create(lv_scr_act());
    lv_obj_set_size(button, lv_pct(35), lv_pct(10));
    auto label = lv_label_create(button);
    lv_label_set_text(label, "Scan WiFi");
    lv_obj_center(label);
    lv_obj_align_to(button, list, LV_ALIGN_OUT_TOP_MID, 0, -5);

    image = lv_image_create(lv_scr_act());
    lv_obj_set_size(image, 50, 50);
    lv_image_set_src(image, LV_SYMBOL_REFRESH);
    lv_obj_align_to(image, list, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_transform_pivot_x(image, 25, 0);
    lv_obj_set_style_transform_pivot_y(image, 25, 0);

    lv_obj_add_event_cb(button, button_event_callback, LV_EVENT_CLICKED, nullptr);
}
