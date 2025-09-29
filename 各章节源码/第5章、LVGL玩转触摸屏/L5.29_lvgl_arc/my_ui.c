#include "my_ui.h"
#include "lvgl.h"

static lv_obj_t* label;

void arc_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const lv_obj_t* arc = lv_event_get_target(e);
        // 获取圆弧的值，并且显示在中间的标签上
        lv_label_set_text_fmt(label, "%d", lv_arc_get_value(arc));
        // 标签的值变化了（尺寸发生变化），对齐方式要再设置一次
        lv_obj_align_to(label, arc, LV_ALIGN_CENTER, 0, 0);
    }
}

void show_ui()
{
    lv_obj_t* arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 150, 150);
    lv_obj_center(arc);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "");
    lv_obj_align_to(label, arc, LV_ALIGN_CENTER, 0, 0);

    // 设置上半圆弧线
    lv_arc_set_bg_angles(arc, 180, 0);

    // 设置圆弧主体的线条宽度为15
    lv_obj_set_style_arc_width(arc, 15, LV_PART_MAIN);
    // 设置圆弧被选中部分的线条的宽度为15
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    // 设置圆弧主体部分颜色为0xe1e1d1
    lv_obj_set_style_arc_color(arc, lv_color_hex(0xe1e1d1), LV_PART_MAIN);
    // 设置圆弧指示器部分颜色为0x1ab2ff
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x1ab2ff), LV_PART_INDICATOR);
    // 设置圆弧中间的操作杆的颜色为0xff0000
    lv_obj_set_style_bg_color(arc, lv_color_hex(0xff0000), LV_PART_KNOB);

    // 设置取值范围
    // lv_arc_set_range(arc, 0, 100);

    // 顺时针旋转90°（注意：精度为1，和之前讲解的图片的旋转精度0.1不一样）
    // lv_arc_set_rotation(arc, 90);

    // 设置初始值
    lv_arc_set_value(arc, 50);

    // 添加事件响应回调函数
    lv_obj_add_event_cb(arc, arc_event_callback, LV_EVENT_VALUE_CHANGED, NULL);
}
