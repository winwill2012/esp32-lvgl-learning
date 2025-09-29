//
// Created by Admin on 2025/7/7.
//

#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    lv_obj_t* obj = lv_obj_create(lv_scr_act());

    lv_obj_set_size(obj, 240, 200);
    lv_obj_set_pos(obj, 10, 20);

    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_state(obj, LV_STATE_DISABLED | LV_STATE_FOCUSED);
    lv_obj_remove_state(obj, LV_STATE_DISABLED);
    lv_obj_has_state(obj, LV_STATE_DISABLED);
}
