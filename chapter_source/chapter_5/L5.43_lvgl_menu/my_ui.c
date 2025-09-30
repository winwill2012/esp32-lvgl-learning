#include "my_ui.h"
#include "lvgl.h"

// 自定义字体
LV_FONT_DECLARE(AlibabaPuHuiTi_Regular_16)

// 自定义的各种小图标
#define LV_CUSTOM_SYMBOL_VOLUME           "\xEE\x98\xA6"
#define LV_CUSTOM_SYMBOL_BRIGHTNESS       "\xEE\x98\x89"
#define LV_CUSTOM_SYMBOL_VOICE            "\xEE\x98\xB3"
#define LV_CUSTOM_SYMBOL_ABOUT            "\xEE\x99\x85"
#define LV_CUSTOM_SYMBOL_BACK            "\xEE\x98\xA3"
#define LV_CUSTOM_SYMBOL_DOWN            "\xEE\x99\x9E"

void show_ui() {
    // 创建一个菜单
    lv_obj_t *menu = lv_menu_create(lv_scr_act());
    // 设置菜单页大小为整个屏幕大小
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    // 设置整个菜单背景颜色为浅灰色
    lv_obj_set_style_bg_color(menu, lv_color_hex(0xF0F0F0), 0);
    // 设置菜单头部固定在顶部
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_FIXED);
    // 如果是菜单最顶层根页面，是否显示返回按钮
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_DISABLED);
    // 设置整个菜单文字字体为自定义字体（支持中文显示）
    lv_obj_set_style_text_font(menu, &AlibabaPuHuiTi_Regular_16, 0);

    // 创建主菜单页
    lv_obj_t *main_page = lv_menu_page_create(menu, "设置");

    // 创建一个分割线
    lv_menu_separator_create(main_page);

    // 创建一个主菜单分区（分区默认设置的布局方式是弹性布局，布局方向是COLUMN）
    lv_obj_t *settings_section = lv_menu_section_create(main_page);
    // 设置分区左右外边距为5
    lv_obj_set_style_margin_hor(settings_section, 5, 0);
    // 设置分区左右内边距为5
    lv_obj_set_style_pad_hor(settings_section, 5, 0);

    // 创建第一个菜单项：音量
    lv_obj_t *volume_label = lv_label_create(settings_section);
    lv_label_set_text(volume_label, LV_CUSTOM_SYMBOL_VOLUME" 音量");
    lv_obj_set_style_margin_ver(volume_label, 10, 0); // 设置菜单项上线外边距为10

    // 创建第二个菜单项：音色
    lv_obj_t *voice_label = lv_label_create(settings_section);
    lv_label_set_text(voice_label, LV_CUSTOM_SYMBOL_VOICE" 音色");
    lv_obj_set_style_margin_ver(voice_label, 10, 0);

    // 创建第三个菜单项：背光
    lv_obj_t *brightness_label = lv_label_create(settings_section);
    lv_label_set_text(brightness_label, LV_CUSTOM_SYMBOL_BRIGHTNESS" 背光");
    lv_obj_set_style_margin_ver(brightness_label, 15, 0);

    // 再创建一个分隔符，把不同的菜单项放到不同分区
    lv_menu_separator_create(main_page);

    // 创建另一个菜单分区
    lv_obj_t *about_section = lv_menu_section_create(main_page);
    lv_obj_set_style_margin_hor(about_section, 5, 0);
    lv_obj_set_style_pad_hor(about_section, 5, 0);

    // 创建第四个菜单项：关于
    lv_obj_t *about_label = lv_label_create(about_section);
    lv_label_set_text(about_label, LV_CUSTOM_SYMBOL_ABOUT" 关于");
    lv_obj_set_style_margin_ver(about_label, 10, 0);

    // 创建音量设置页面
    lv_obj_t *volume_page = lv_menu_page_create(menu, "音量设置");
    // 创建一个菜单容器，菜单容器的默认布局方式为弹性布局，方向为ROW
    lv_obj_t *cont = lv_menu_cont_create(volume_page);
    lv_obj_set_style_margin_top(cont, 5, 0);
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, LV_CUSTOM_SYMBOL_VOLUME" 音量");
    lv_obj_t *volume_slider = lv_slider_create(cont);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_obj_set_height(volume_slider, 5);

    lv_obj_set_flex_grow(label, 2); // label和volume_slider水平排列，label占2/7空间
    lv_obj_set_flex_grow(volume_slider, 5); // volume_slider占5/7空间
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xffffff), 0);

    // 创建音色设置页面
    lv_obj_t *voice_page = lv_menu_page_create(menu, "音色选择");
    cont = lv_menu_cont_create(voice_page);
    lv_obj_set_style_margin_top(cont, 5, 0);
    label = lv_label_create(cont);
    lv_label_set_text(label, LV_CUSTOM_SYMBOL_VOICE" 音色");
    lv_obj_t *voice_dropdown = lv_dropdown_create(cont);
    lv_dropdown_set_options(voice_dropdown, "弯弯小何\n四郎\n御姐");
    lv_dropdown_set_symbol(voice_dropdown, LV_CUSTOM_SYMBOL_DOWN);

    lv_obj_set_style_text_font(voice_dropdown, &AlibabaPuHuiTi_Regular_16, 0);
    lv_obj_t *voice_dropdown_list = lv_dropdown_get_list(voice_dropdown);
    lv_obj_set_style_text_font(voice_dropdown_list, &AlibabaPuHuiTi_Regular_16, 0);

    lv_obj_set_flex_grow(label, 2);
    lv_obj_set_flex_grow(voice_dropdown, 5);

    // 创建背光设置页面
    lv_obj_t *brightness_page = lv_menu_page_create(menu, "背光调节");
    cont = lv_menu_cont_create(brightness_page);
    lv_obj_set_style_margin_top(cont, 5, 0);
    label = lv_label_create(cont);
    lv_label_set_text(label, LV_CUSTOM_SYMBOL_BRIGHTNESS" 背光");
    lv_obj_t *brightness_slider = lv_slider_create(cont);
    lv_slider_set_range(brightness_slider, 0, 100);
    lv_obj_set_height(brightness_slider, 5);

    lv_obj_set_flex_grow(label, 2);
    lv_obj_set_flex_grow(brightness_slider, 5);

    // 创建关于设置页面
    lv_obj_t *about_page = lv_menu_page_create(menu, "关于我们");
    cont = lv_menu_cont_create(about_page);
    lv_obj_set_style_margin_top(cont, 5, 0);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, 这里是微联编程");

    // 设置主菜单页面
    // lv_menu_set_sidebar_page(menu, main_page);  // 可以通过这个函数设置显示的侧边栏页面
    lv_menu_set_page(menu, main_page);

    // 设置头部的返回按钮的图片为自定义的图片
    // 因为全局设置了menu的字体为自定义字体，用LVGL默认返回图标会无法显示
    const lv_obj_t *button = lv_menu_get_main_header_back_button(menu);
    lv_image_set_src(lv_obj_get_child(button, 0), LV_CUSTOM_SYMBOL_BACK);

    // 点击相应的标签，跳转到对应的页面
    lv_menu_set_load_page_event(menu, volume_label, volume_page);
    lv_menu_set_load_page_event(menu, brightness_label, brightness_page);
    lv_menu_set_load_page_event(menu, voice_label, voice_page);
    lv_menu_set_load_page_event(menu, about_label, about_page);
}
