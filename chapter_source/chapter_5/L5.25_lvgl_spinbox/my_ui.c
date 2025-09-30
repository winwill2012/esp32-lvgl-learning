#include "my_ui.h"
#include "lvgl.h"

// 全局微调框对象
static lv_obj_t* spinbox;

// 点击减号按钮的回调函数
void decrement_button_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        // 调用微调框减小数值函数
        lv_spinbox_decrement(spinbox);
    }
}

// 点击加号按钮的回调函数
void increment_button_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        // 调用微调框增加数值的函数
        lv_spinbox_increment(spinbox);
    }
}

void show_ui()
{
    // 创建微调框，设置大小，设置居中
    spinbox = lv_spinbox_create(lv_scr_act());
    lv_obj_set_size(spinbox, 80, 40);
    lv_obj_center(spinbox);

    // 设置文字居中显示
    lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // 设置数字总位数为3，其中整数部分为2位，有一位小数
    lv_spinbox_set_digit_format(spinbox, 3, 2);

    // 设置数据范围0~100，有一位小数，所以其实数值范围是[0, 10.0]
    lv_spinbox_set_range(spinbox, 0, 100);

    // 设置增长步长为1，实际是0.1
    // 主要：步长只在非编辑模式（不手动改变指针）时生效
    // 如果手动点击了spinbox上的数字指针，步长将变成1
    lv_spinbox_set_step(spinbox, 2);

    // 设置当前值为50，实际是5.0
    lv_spinbox_set_value(spinbox, 50);

    // 设置微调框背景颜色的淡蓝色
    lv_obj_set_style_bg_color(spinbox, lv_color_hex(0x33cccc), LV_PART_MAIN);

    // 在微调框两侧创建两个按钮，用于增加或者减小数值
    lv_obj_t* decrement_button = lv_button_create(lv_scr_act());
    lv_obj_t* increment_button = lv_button_create(lv_scr_act());
    lv_obj_set_size(decrement_button, 40, 40);
    lv_obj_set_size(increment_button, 40, 40);
    lv_obj_t* decrement_label = lv_label_create(decrement_button);
    lv_obj_t* increment_label = lv_label_create(increment_button);
    lv_label_set_text(decrement_label, LV_SYMBOL_MINUS);
    lv_label_set_text(increment_label, LV_SYMBOL_PLUS);
    lv_obj_center(decrement_label);
    lv_obj_center(increment_label);

    // 分别设置两个按钮的背景颜色
    lv_obj_set_style_bg_color(decrement_button, lv_color_hex(0xff3300), LV_PART_MAIN);
    lv_obj_set_style_bg_color(increment_button, lv_color_hex(0x33cc33), LV_PART_MAIN);

    // 两个按钮分别位于微调框左右两边
    lv_obj_align_to(decrement_button, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_align_to(increment_button, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    // 为微调框增加事件回调函数
    lv_obj_add_event_cb(decrement_button, decrement_button_event_callback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(increment_button, increment_button_event_callback, LV_EVENT_CLICKED, NULL);
}
