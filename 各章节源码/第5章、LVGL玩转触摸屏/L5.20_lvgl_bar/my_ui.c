#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    // 创建进度条
    lv_obj_t* bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(bar, 250, 15); // 当宽度大于高度时，进度条为水平方向
    // lv_obj_set_size(bar, 15, 250); // 当高度大于宽度时，进度条为垂直方向
    // lv_obj_set_size(bar, 200, 200); // 当高度等于宽度时，进度条为圆形
    // 进度条居中
    lv_obj_center(bar);

    // 普通模式
    lv_bar_set_mode(bar, LV_BAR_MODE_NORMAL);  // 普通模式
    // lv_bar_set_mode(bar, LV_BAR_MODE_SYMMETRICAL); // 对称模式，对称模式下一般最小值和最大值要互为相反数
    // lv_bar_set_mode(bar, LV_BAR_MODE_RANGE); // 范围模式

    // 设置进度条范围
    lv_bar_set_range(bar, -100, 100);
    // 设置动画时间为500ms
    lv_obj_set_style_anim_duration(bar, 500, LV_PART_MAIN);

    // 设置进度条指示器背景颜色为红色
    lv_obj_set_style_bg_color(bar, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    // 设置进度条指示器渐变到蓝色
    lv_obj_set_style_bg_grad_color(bar, lv_color_hex(0x0000FF), LV_PART_INDICATOR);
    // 设置进度条背景色渐变的方向是水平渐变
    lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);

    // 范围模式下，设置区间左值（注意：该函数必须在范围模式下才生效）
    // lv_bar_set_start_value(bar, -50, LV_ANIM_ON);
    lv_bar_set_value(bar, 50, LV_ANIM_ON);
}
