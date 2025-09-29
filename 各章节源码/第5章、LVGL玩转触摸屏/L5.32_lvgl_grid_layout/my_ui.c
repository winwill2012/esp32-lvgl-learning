#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    // 创建父节点容器
    lv_obj_t* container = lv_obj_create(lv_scr_act());
    // 设置容器宽度为屏幕100%，高度为屏幕80%
    lv_obj_set_size(container, lv_pct(100), lv_pct(80));
    // 将容器背景颜色设置为浅灰色
    lv_obj_set_style_bg_color(container, lv_color_hex(0XEEEEEE), 0);
    // 移除容器默认的内部填充
    lv_obj_set_style_pad_all(container, 0, 0);
    // 将容器居中展示
    lv_obj_center(container);

    // 创建网格布局的列数组（一共三列，每一列占全部空间的一份，也就是三列宽度相等）
    static int32_t col_size[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
    };

    // 创建网格布局的行数组（一共四行，每一行占全部空间的一份，也就是四行高度相等）
    static int32_t row_size[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
    };

    // 设置父容器布局模式为网格布局
    lv_obj_set_layout(container, LV_LAYOUT_GRID);
    // 设置行列尺寸
    lv_obj_set_grid_dsc_array(container, col_size, row_size);
    // 设置子元素整体在父容器内的对齐方式（左对齐，顶部对齐）
    lv_obj_set_grid_align(container, LV_GRID_ALIGN_START, LV_GRID_ALIGN_START);

    // 创建11个按钮
    for (int i = 0; i < 11; i++)
    {
        // 计算每一个按钮的行列坐标
        const int32_t col = i % 3;
        const int32_t row = i / 3;

        lv_obj_t* btn = lv_button_create(container);
        lv_obj_t* label = lv_label_create(btn);
        if (i < 10)
        {
            lv_label_set_text_fmt(label, "%d", (i + 1) % 10);
            lv_obj_center(label);
            // 如果是前面10个数字按钮，在行列两个方向都只占用一个位置，并且对象大小自动拉伸到与宫格尺寸一致
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
        }
        else
        {
            lv_label_set_text(label, "OK");
            lv_obj_center(label);
            lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
            // 如果是OK字按钮，占两列，占一行，并且对象大小自动拉伸到与宫格尺寸一致
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 2, LV_GRID_ALIGN_STRETCH, row, 1);
        }
    }
}
