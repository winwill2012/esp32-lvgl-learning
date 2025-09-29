//
// Created by Admin on 2025/7/7.
//

#include "my_ui.h"
#include "lvgl.h"

void event_callback(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* button = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(button, 0);
    char* text = lv_label_get_text(label);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        if (lv_strcmp(text, LV_SYMBOL_POWER" ON") == 0)
        {
            lv_label_set_text(label, LV_SYMBOL_POWER" OFF");
            lv_obj_set_style_bg_color(button, lv_color_hex(0xFF0000), 0);
        } else
        {
            lv_label_set_text(label, LV_SYMBOL_POWER" ON");
            lv_obj_set_style_bg_color(button, lv_color_hex(0x00AA00), 0);
        }
        break;
    default:
        break;
    }
}

void show_ui()
{
    lv_obj_t* button = lv_button_create(lv_scr_act());
    lv_obj_set_size(button, 100, 30);
    lv_obj_center(button);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_center(label);
    lv_label_set_text(label, LV_SYMBOL_POWER" ON");
    lv_obj_set_style_bg_color(button, lv_color_hex(0x00AA00), 0);
    lv_obj_add_event_cb(button, event_callback, LV_EVENT_CLICKED, NULL);
}
