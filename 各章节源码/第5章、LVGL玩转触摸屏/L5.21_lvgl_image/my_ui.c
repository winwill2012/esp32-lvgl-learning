#include "my_ui.h"
#include "lvgl.h"
#include "IMG_202507257430_240x320.h"

void show_ui()
{
    // 创建图片组件
    lv_obj_t* image = lv_image_create(lv_scr_act());
    lv_obj_set_size(image, 240, 320);
    lv_obj_center(image);

    // 显示内置图标
    // lv_image_set_src(image, LV_SYMBOL_WIFI" WiFi Settings");
    // 显示自定义图片
    lv_image_set_src(image, &IMG_202507257430_240x320);

    // 设置图片透明度
    lv_obj_set_style_opa(image, 255, LV_PART_MAIN);

    // 设置图片放大系统（256为原图, 128为50%，512为200%）
    lv_image_set_scale(image, 256);

    // 图片颜色滤镜
    lv_obj_set_style_image_recolor(image, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(image, 50, LV_PART_MAIN);

    // 图片旋转
    lv_image_set_pivot(image, 0, 0);
    lv_image_set_rotation(image, 450);
}
