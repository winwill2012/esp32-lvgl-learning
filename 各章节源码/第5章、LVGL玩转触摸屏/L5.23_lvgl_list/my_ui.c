#include "my_ui.h"
#include "lvgl.h"

static int wifi_count = 0;
static lv_obj_t* list;
static lv_obj_t* label;

void list_button_event_callback(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        const char* text = lv_list_get_button_text(list,btn);
        lv_label_set_text(label, text);
        lv_obj_align_to(label, list, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    }
}

void delete_button_event_callback(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (wifi_count > 0)
        {
            lv_obj_t* btn = lv_obj_get_child(list, --wifi_count);
            lv_obj_delete(btn);
        }
    }
}

void add_button_event_callback(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        char wifi_name[25];
        lv_snprintf(wifi_name, sizeof(wifi_name), "WiFi_%d", ++wifi_count);
        lv_obj_t* btn = lv_list_add_button(list, LV_SYMBOL_WIFI, wifi_name);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0099cc), 0);
        lv_obj_scroll_to_view(btn, LV_ANIM_ON );
        lv_obj_add_event_cb(btn, list_button_event_callback, LV_EVENT_CLICKED, NULL);
    }
}


void show_ui()
{
    list = lv_list_create(lv_scr_act());
    lv_obj_set_size(list, 220, 250);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_set_style_pad_all(list, 0, 0);

    lv_obj_t* delete_button = lv_button_create(lv_scr_act());
    lv_obj_t* add_button = lv_button_create(lv_scr_act());

    lv_obj_set_size(delete_button, 40, 25);
    lv_obj_set_size(add_button, 40, 25);

    lv_obj_t* delete_label = lv_label_create(delete_button);
    lv_obj_t* add_label = lv_label_create(add_button);
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "");

    lv_label_set_text(delete_label, LV_SYMBOL_MINUS);
    lv_label_set_text(add_label, LV_SYMBOL_PLUS);

    lv_obj_align_to(delete_button, list, LV_ALIGN_OUT_BOTTOM_LEFT, 5, 5);
    lv_obj_align_to(add_button, list, LV_ALIGN_OUT_BOTTOM_RIGHT, -5, 5);
    lv_obj_align_to(label, list, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    //颜色拾色器： https://www.w3cschool.cn/tools/index?name=cpicker
    lv_obj_set_style_bg_color(delete_button, lv_color_hex(0Xff3300), 0);
    lv_obj_set_style_bg_color(add_button, lv_color_hex(0X00cc66), 0);

    lv_obj_center(delete_label);
    lv_obj_center(add_label);

    lv_obj_add_event_cb(delete_button, delete_button_event_callback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(add_button, add_button_event_callback, LV_EVENT_CLICKED, NULL);
}
