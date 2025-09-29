//
// Created by Admin on 2025/7/7.
//
#include "my_ui.h"
#include "lvgl.h"

static lv_obj_t* button;

void sw_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
        {
            const lv_obj_t* sw = (lv_obj_t*)lv_event_get_target(e);
            bool isOpened = lv_obj_has_state(sw, LV_STATE_CHECKED);
            if (button == NULL)
            {
                button = lv_button_create(lv_scr_act());
                lv_obj_set_size(button, 50, 30);
                lv_obj_align(button, LV_ALIGN_CENTER, 0, -100);
            }
            if (isOpened)
            {
                // 显示按钮
                lv_obj_remove_flag(button, LV_OBJ_FLAG_HIDDEN);
            } else
            {
                // 隐藏按钮
                lv_obj_add_flag(button, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        }
    default:
        break;
    }
}

void show_ui()
{
    lv_obj_t* sw = lv_switch_create(lv_scr_act());
    lv_obj_set_size(sw, 100, 30);
    lv_obj_center(sw);

    lv_obj_set_style_bg_color(sw, lv_color_hex(0x00EE00), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xEE0000), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x00FFFF), LV_PART_KNOB);

    lv_obj_t* left_label = lv_label_create(lv_scr_act());
    lv_obj_t* right_label = lv_label_create(lv_scr_act());
    lv_label_set_text(left_label, "hide");
    lv_label_set_text(right_label, "show");
    lv_obj_align_to(left_label, sw, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_align_to(right_label, sw, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_add_event_cb(sw, sw_event_callback, LV_EVENT_VALUE_CHANGED, NULL);
}
