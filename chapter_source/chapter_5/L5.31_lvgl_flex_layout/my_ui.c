#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    // 创建一个容器
    lv_obj_t* container = lv_obj_create(lv_scr_act());
    // 设置容器大小为240x240
    lv_obj_set_size(container, 240, 240);
    // 设置容器背景颜色为浅灰色
    lv_obj_set_style_bg_color(container, lv_color_hex(0xEEEEEE), 0);
    // 移除容器默认的内部填充样式
    lv_obj_set_style_pad_all(container, 0, 0);
    // 容器居中显示
    lv_obj_center(container);

    // 设置容器的布局方式为弹性布局
    lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    // 设置弹性布局的主轴方向为水平方向（行优先），并且空间不够时自动换行
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    // 依次设置子元素在主轴、交叉轴、整体方向的对齐方式
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    // 循环创建9个按钮
    for (int i = 0; i < 9; i++)
    {
        lv_obj_t* btn = lv_button_create(container);
        lv_obj_set_style_margin_all(btn, 0, 0);
        // 为按钮设置不同的高度，以便观察交叉轴方向的对齐效果
        lv_obj_set_size(btn, 70, i % 2 ? 40 : 50);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%d", i + 1);
        lv_obj_center(label);
    }
}
