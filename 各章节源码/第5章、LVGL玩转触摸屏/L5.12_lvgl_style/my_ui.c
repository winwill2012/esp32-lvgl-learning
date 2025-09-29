//
// Created by Admin on 2025/7/7.
//

#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, 240, 200);
    lv_obj_set_align(obj, LV_ALIGN_CENTER);

    // // //***************方法一：先创建样式，再应用样式，优点是方便将样式复用到其他对象********************
    // // // 这里的static必须加上，如果不加就是局部变量，show_ui函数调用结束之后就销毁了，会导致程序crash
    // static lv_style_t style;
    // lv_style_init(&style);
    // //
    // // 基本设置
    // lv_style_set_bg_color(&style, lv_color_hex(0xDDDDDD)); // 设置背景颜色为浅灰色
    // lv_style_set_border_width(&style, 10); // 设置边框宽度为10
    // lv_style_set_border_color(&style, lv_color_hex(0xFF0000)); // 设置边框颜色为红色
    // lv_style_set_radius(&style, 5); // 增加5个像素的圆角
    // lv_style_set_pad_all(&style, 10); // 内边距设置为10像素
    // //
    // // 设置轮廓
    // lv_style_set_outline_width(&style, 1); // 设置轮廓的宽度为1
    // lv_style_set_outline_color(&style, lv_color_hex(0x0000FF)); // 设置轮廓为蓝色
    // lv_style_set_outline_opa(&style, 128); // 设置轮廓的透明度
    // lv_style_set_outline_pad(&style, 10); // 设置轮廓与边框之间的填充距离
    // //
    // // 设置边缘阴影
    // lv_style_set_shadow_width(&style, 100); // 设置边缘阴影宽度为10
    // lv_style_set_shadow_color(&style, lv_color_hex(0x00FF00)); // 设置边缘阴影颜色为绿色
    // lv_style_set_shadow_opa(&style, 255); // 设置边缘阴影透明度为128
    // lv_style_set_shadow_offset_x(&style, 10); // 阴影向右偏移10
    // lv_style_set_shadow_offset_y(&style, -10); // 阴影向上偏移10
    // //
    // // // 为对象绑定样式
    // lv_obj_add_style(obj, &style, 0);

    // ***************方法二：使用lv_obj_set_style_xxx风格*************************************
    // 基本设置
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xDDDDDD), 0);         // 设置背景颜色为浅灰色
    lv_obj_set_style_border_width(obj, 10, 0);                          // 设置边框宽度为10
    lv_obj_set_style_border_color(obj, lv_color_hex(0xFF0000), 0);     // 设置边框颜色为红色
    lv_obj_set_style_radius(obj, 5, 0);                                 // 增加5个像素的圆角
    lv_obj_set_style_pad_all(obj, 10, 0);                               // 内边距设置为10像素

    // 设置轮廓
    lv_obj_set_style_outline_width(obj, 1, 0);                          // 设置轮廓的宽度为1
    lv_obj_set_style_outline_color(obj, lv_color_hex(0x0000FF), 0);    // 设置轮廓为蓝色
    lv_obj_set_style_outline_opa(obj, 128, 0);                          // 设置轮廓的透明度
    lv_obj_set_style_outline_pad(obj, 20, 0);                           // 设置轮廓与边框之间的填充距离

    // 设置边缘阴影
    lv_obj_set_style_shadow_width(obj, 10, 0);                          // 设置边缘阴影宽度为10
    lv_obj_set_style_shadow_color(obj, lv_color_hex(0x00FF00), 0);     // 设置边缘阴影颜色为绿色
    lv_obj_set_style_shadow_opa(obj, 128, 0);                           // 设置边缘阴影透明度为128
    lv_obj_set_style_shadow_ofs_x(obj, 10, 0);                          // 阴影向右偏移10
    lv_obj_set_style_shadow_ofs_y(obj, -10, 0);                         // 阴影向上偏移10
}
