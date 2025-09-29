//
// Created by Admin on 2025/7/7.
//
#include "my_ui.h"
#include "lvgl.h"

// 定义两个全局标签，用于显示当前滑块的左右值
static lv_obj_t* left_label;
static lv_obj_t* right_label;

void slider_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    const lv_obj_t* slider = (lv_obj_t*)lv_event_get_target(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
        {
            // 获取滑块左值
            const int32_t left_value = lv_slider_get_left_value(slider);
            // 获取滑块右值
            const int32_t right_value = lv_slider_get_value(slider);
            // 第一次时创建左值显示标签
            if (left_label == NULL)
            {
                left_label = lv_label_create(lv_scr_act());
                lv_obj_align_to(left_label, slider, LV_ALIGN_OUT_TOP_MID, -50, -20);
            }
            // 第一次时创建右值显示标签
            if (right_label == NULL)
            {
                right_label = lv_label_create(lv_scr_act());
                lv_obj_align_to(right_label, slider, LV_ALIGN_OUT_TOP_MID, 50, -20);
            }
            lv_label_set_text_fmt(left_label, "%d", left_value);
            lv_label_set_text_fmt(right_label, "%d", right_value);
            break;
        }
    default:
        break;
    }
}

#define MIN_VALUE (-100)   // 滑块最小值，如果滑块模式是对称模式，一般最大值和最小值为相反数
#define MAX_VALUE 100      // 滑块最大值，如果滑块模式是对称模式，一般最大值和最小值为相反数

void show_ui()
{
    // 创建滑块
    lv_obj_t* slider = lv_slider_create(lv_scr_act());
    // 设置滑块尺寸
    lv_obj_set_size(slider, 200, 10);
    // 让滑块居中
    lv_obj_center(slider);
    // 设置滑块的最大值和最小值（如果滑块模式是对称模式，则最大值和最小值一般为相反数）
    lv_slider_set_range(slider, MIN_VALUE, MAX_VALUE);

    // // 设置滑块初始值
    lv_slider_set_value(slider, 20, LV_ANIM_ON);  // 右值

    // 创建两个标签，用于标识范围值
    lv_obj_t* left_label = lv_label_create(lv_scr_act());
    lv_obj_t* right_label = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(left_label, "%d", MIN_VALUE);
    lv_label_set_text_fmt(right_label, "%d", MAX_VALUE);
    // 标签相对于滑块垂直居中对齐，一个位于左边，一个位于右边
    lv_obj_align_to(left_label, slider, LV_ALIGN_OUT_LEFT_MID, -15, 0);
    lv_obj_align_to(right_label, slider, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

    // 设置滑块主体背景颜色为蓝色
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x0000FF), LV_PART_MAIN);
    // 设置滑块指示器部分颜色为红色
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    // 设置滑块旋钮部分为绿色
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x00AA00), LV_PART_KNOB);

    // // 设置滑块模式
    // lv_slider_set_mode(slider, LV_SLIDER_MODE_NORMAL); // 普通模式，只有一个滑块，调整一个绝对数值
    // lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL); // 对称模式，相对于中心点0的偏移量
    lv_slider_set_mode(slider, LV_SLIDER_MODE_RANGE); // 范围模式，两个滑块，可以分别调整左右值
    lv_slider_set_left_value(slider, -20, LV_ANIM_ON);  // 设置初始左值（先设置为范围模式才会生效）

    // 为滑块添加事件回调函数
    lv_obj_add_event_cb(slider, slider_event_callback, LV_EVENT_VALUE_CHANGED, 0);
}
