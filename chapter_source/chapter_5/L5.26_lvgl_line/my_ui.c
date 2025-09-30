#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    // 1. 定义线段的点坐标（可以是多个点，形成折线）
    // 注意：这里的这些点必须设置为static，防止函数调用结束后，内存被回收
    static lv_point_precise_t points[] = {
        {0, 0}, // 起点 (x, y)
        {200, 0}, // 第2个点
        {200, 200}, // 第3个点
        {0, 200}, // 第4个点
        {0, 0}, // 第5个点
        {200, 200}, // 第6个点
        {0, 200}, // 第7个点
        {200, 0}, // 第8个点
        {100, 0}, // 第9个点
        {100, 200}, // 第10个点
        {0, 200}, // 第11个点
        {0, 100}, // 第12个点
        {200, 100}, // 第13个点
    };
    // 2. 创建线段对象
    lv_obj_t* line = lv_line_create(lv_scr_act()); // 在当前屏幕创建

    // 3. 设置线段的点集
    lv_line_set_points(line, points, sizeof(points) / sizeof(lv_point_precise_t));

    lv_obj_set_style_line_color(line, lv_color_hex(0xFF0000), LV_PART_MAIN); // 线段颜色设置为红色
    lv_obj_set_style_line_width(line, 2, LV_PART_MAIN); // 线宽2像素
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN); // 线段拐角圆角

    // 设置虚线效果
    // 注意： 虚线只能是水平或者垂直方向
    lv_obj_set_style_line_dash_width(line, 5, LV_PART_MAIN); // 设置虚线每一段长度
    lv_obj_set_style_line_dash_gap(line, 2, LV_PART_MAIN); // 设置虚线相邻两端间隔

    // 将整个线段对象居中，这会导致整体进行相对位置的偏移
    lv_obj_center(line);
}
