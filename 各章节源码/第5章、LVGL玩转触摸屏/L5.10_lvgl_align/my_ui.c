//
// Created by Admin on 2025/7/7.
//

#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    lv_obj_t* obj1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj1, 240, 200);
    lv_obj_set_style_pad_all(obj1, 0, 0);
    lv_obj_set_align(obj1, LV_ALIGN_CENTER);

    lv_obj_t* obj2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj2, 100, 100);
    lv_obj_align_to(obj2, obj1, LV_ALIGN_TOP_LEFT, 0, 0);
}
