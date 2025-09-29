#include "my_ui.h"
#include "lvgl.h"

static lv_obj_t* label;

void table_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* table = lv_event_get_target(e);
    if (code == LV_EVENT_FOCUSED)
    {
        // 获取表格中当前被点击的单元格的坐标
        uint32_t row, col;
        lv_table_get_selected_cell(table, &row, &col);
        // 拿到指定坐标的单元格的值
        const char* value = lv_table_get_cell_value(table, row, col);
        // 显示在标签上
        lv_label_set_text(label, value);
        lv_obj_align_to(label, table, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    }
}
void show_ui()
{
    lv_obj_t* table = lv_table_create(lv_scr_act());
    lv_obj_set_size(table, 220, 250);
    lv_obj_center(table);

    lv_table_set_column_count(table, 2);

    lv_table_set_column_width(table, 0, 150);
    lv_table_set_column_width(table, 1, 70);

    // 设置单元格字体对齐方式为居中对齐
    lv_obj_set_style_text_align(table, LV_TEXT_ALIGN_CENTER, LV_PART_ITEMS);
    // 设置单元格被选中时，字体的颜色变成红色
    lv_obj_set_style_text_color(table, lv_color_hex(0xFF0000), LV_PART_ITEMS | LV_STATE_FOCUSED);

    // 设置第一行
    lv_table_set_cell_value(table, 0, 0, "Student Info");

    // 设置第二行
    lv_table_set_cell_value(table, 1, 0, "Name");
    lv_table_set_cell_value_fmt(table, 1, 1, "Age");

    // 设置第三行
    lv_table_set_cell_value(table, 2, 0, "ZhangSan");
    lv_table_set_cell_value_fmt(table, 2, 1, "%d", 12);

    // 设置第四行
    lv_table_set_cell_value(table, 3, 0, "Lisi");
    lv_table_set_cell_value_fmt(table, 3, 1, "%d", 23);

    // 设置第五行
    lv_table_set_cell_value(table, 4, 0, "Wangwu");
    lv_table_set_cell_value_fmt(table, 4, 1, "%d", 19);

    // 设置第一行、第一列与右边得到单元格合并
    lv_table_set_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);

    // 创建一个标签，用于展示表格上被点击的单元格的内容
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "");
    lv_obj_align_to(label, table, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    // 为表格增加事件回调函数
    lv_obj_add_event_cb(table, table_event_callback, LV_EVENT_FOCUSED, NULL);
}
