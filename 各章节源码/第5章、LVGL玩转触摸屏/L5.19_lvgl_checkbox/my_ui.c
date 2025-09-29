#include "my_ui.h"
#include "lvgl.h"

void checkbox_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* checkbox = (lv_obj_t*)lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // 判断当前复选框是否是被选中状态
        if (lv_obj_has_state(checkbox, LV_STATE_CHECKED))
        {
            lv_checkbox_set_text(checkbox, "checked");
        }
        else
        {
            lv_checkbox_set_text(checkbox, "unchecked");
        }
    }
}

void show_ui()
{
    // 创建复选框
    lv_obj_t* checkbox = lv_checkbox_create(lv_scr_act());

    // 注意：复选框不需要设置尺寸，互已根据文字内容自动会调整尺寸大小
    // lv_obj_set_size(checkbox, 200, 20);

    // 设置复选框文字内容为Apple
    lv_checkbox_set_text(checkbox, "checked");

    // 复选框居中
    lv_obj_center(checkbox);

    // 设置默认状态为选中状态
    lv_obj_add_state(checkbox, LV_STATE_CHECKED);

    // 设置复选框文字内容为红色
    lv_obj_set_style_text_color(checkbox, lv_color_hex(0xFF0000), LV_PART_MAIN);

    // 设置整个复选框空间的透明度为100（最大值255）
    lv_obj_set_style_opa(checkbox, 100, LV_PART_MAIN);

    // 设置复选框方框的圆角为20像素
    lv_obj_set_style_radius(checkbox, 20, LV_PART_INDICATOR);

    // 添加事件响应回调函数
    lv_obj_add_event_cb(checkbox, checkbox_event_callback, LV_EVENT_VALUE_CHANGED, NULL);
}
