#include "my_ui.h"
#include "lvgl.h"

lv_obj_t * selected_music_label;

void dropdown_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    const lv_obj_t* dropdown = lv_event_get_target(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
        {
            // 获取当前选中的选项的索引（从0开始）
            const uint32_t index = lv_dropdown_get_selected(dropdown);
            char option[128];
            // 获取当前选中的选项的文字内容
            lv_dropdown_get_selected_str(dropdown, option, 128);
            if (selected_music_label == NULL)
            {
                selected_music_label = lv_label_create(lv_scr_act());
                lv_obj_align(selected_music_label, LV_ALIGN_TOP_MID, 0, 10);
            }
            lv_label_set_text_fmt(selected_music_label, "%d: %s",index, option);
            break;
        }
    default:
        break;
    }
}

void show_ui()
{
    // 创建一个下拉选项
    lv_obj_t* dropdown = lv_dropdown_create(lv_scr_act());
    // 设置下拉选项尺寸
    lv_obj_set_size(dropdown, 200, 25);
    // 设置下拉选项位置居中然后上移50像素
    lv_obj_align(dropdown, LV_ALIGN_CENTER, -120, -50);
    // 设置下拉选项显示的默认文字内容
    lv_dropdown_set_text(dropdown, "Please select music");
    // 设置下拉选项右边的符号
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_AUDIO);
    // 设置下拉选项得到条目
    lv_dropdown_set_options(dropdown, "Hey Jude\nYesterday\nShape of You\nSmells Like Teen Spirit");
    // 在现有条目中新增条目，注意：第二个参数不要添加换行符
    lv_dropdown_add_option(dropdown, "Sweet Child O' Mine", 0);
    // 设置列表展开方向
    lv_dropdown_set_dir(dropdown, LV_DIR_RIGHT);

    // 设置下拉框按钮上的文字颜色为红色（这里会同时设置符号为红色，下面的函数可以单独这是符号颜色）
    lv_obj_set_style_text_color(dropdown, lv_color_hex(0xFF0000), LV_PART_MAIN);
    // 设置下拉框符号颜色为黑色
    lv_obj_set_style_text_color(dropdown, lv_color_hex(0x000000), LV_PART_INDICATOR);
    // 设置下拉框方形圆角为0（去除圆角）
    lv_obj_set_style_radius(dropdown, 0, LV_PART_MAIN);
    // 获取下拉框列表
    lv_obj_t* dropdown_list = lv_dropdown_get_list(dropdown);
    // 设置下拉框列表的字体颜色为绿色
    lv_obj_set_style_text_color(dropdown_list, lv_color_hex(0x00FF00), LV_PART_MAIN);
    // 设置下拉框列表中文字对齐方式为居中对齐
    lv_obj_set_style_text_align(dropdown_list, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // 为下拉框添加事件回调函数
    lv_obj_add_event_cb(dropdown, dropdown_event_callback, LV_EVENT_VALUE_CHANGED, 0);
}
