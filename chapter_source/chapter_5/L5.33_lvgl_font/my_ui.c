#include "my_ui.h"
#include "lvgl.h"

LV_FONT_DECLARE(Alibaba_PuHuiTi_Regular_14)

void show_ui()
{
    lv_obj_t* button = lv_button_create(lv_screen_active());
    lv_obj_set_size(button, 100, 30);
    lv_obj_center(button);
    lv_obj_t* label = lv_label_create(button);
    lv_label_set_text(label, "ABC点击123");
    lv_obj_set_style_text_font(label, &Alibaba_PuHuiTi_Regular_14, 0);
    lv_obj_center(label);
}
