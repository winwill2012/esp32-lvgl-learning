#include "my_ui.h"
#include "lvgl.h"

LV_FONT_DECLARE(iconfont_40)
#define LV_CUSTOM_SYMBOL_LIGHT          "\xEE\x98\xBA"

void show_ui()
{
    lv_obj_t* button = lv_button_create(lv_screen_active());
    lv_obj_set_size(button, 100, 50);
    lv_obj_center(button);
    lv_obj_t* label = lv_label_create(button);
    lv_label_set_text(label, LV_CUSTOM_SYMBOL_LIGHT);
    lv_obj_set_style_text_font(label, &iconfont_40, 0);
    lv_obj_center(label);
}
