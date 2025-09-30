#include "screen_settings.h"
#include "src/core/lv_obj.h"
#include "src/widgets/label/lv_label.h"
#include "src/widgets/menu/lv_menu.h"
#include "gui.h"
#include "screen_settings_about.h"
#include "screen_settings_network.h"
#include "screen_settings_llm.h"
#include "screen_settings_reset.h"

void menu_event_callback(lv_event_t *event) {
    const lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t *button = lv_event_get_target(event);
        if (lv_menu_back_button_is_root(lv_ui.menu, button)) {
            lv_screen_load_anim(lv_ui.screen_home, LV_SCR_LOAD_ANIM_OVER_RIGHT, 300, 0, false);
        }
    }
}

void setup_screen_settings() {
    lv_ui.screen_settings = lv_obj_create(NULL);
    lv_obj_set_style_text_font(lv_ui.screen_settings, &AlibabaPuHuiTi_Regular_15, 0);
    lv_ui.menu = lv_menu_create(lv_ui.screen_settings);
    // 菜单高度为屏幕高度减去顶部状态栏的高度
    lv_obj_set_size(lv_ui.menu, LV_HOR_RES, LV_VER_RES - STATUS_BAR_HEIGHT);
    lv_obj_set_style_bg_color(lv_ui.menu, lv_color_hex(0xF0F0F0), 0);
    // 菜单在屏幕底部对齐
    lv_obj_align(lv_ui.menu, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_menu_set_mode_header(lv_ui.menu, LV_MENU_HEADER_TOP_FIXED);
    // 菜单根页面是否显示返回按钮
    lv_menu_set_mode_root_back_button(lv_ui.menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);

    // 创建设置主界面
    lv_ui.menu_main_page = lv_menu_page_create(lv_ui.menu, "设置");
    lv_ui.menu_main_page_seperator1 = lv_menu_separator_create(lv_ui.menu_main_page);

    // 创建样式，设置每一个菜单项上下外边距和内边距
    lv_style_init(&lv_ui.menu_item_style);
    lv_style_set_margin_ver(&lv_ui.menu_item_style, 5);
    lv_style_set_pad_all(&lv_ui.menu_item_style, 5);

    // 创建菜单项分区
    lv_ui.menu_main_page_section_main = lv_menu_section_create(lv_ui.menu_main_page);
    lv_obj_set_style_margin_hor(lv_ui.menu_main_page_section_main, 5, 0);
    lv_obj_set_style_pad_hor(lv_ui.menu_main_page_section_main, 5, 0);

    // 网络设置菜单项
    lv_ui.menu_main_page_label_network = lv_label_create(lv_ui.menu_main_page_section_main);
    lv_label_set_text(lv_ui.menu_main_page_label_network, LV_CUSTOM_SYMBOL_WIFI " 网络");
    lv_obj_add_style(lv_ui.menu_main_page_label_network, &lv_ui.menu_item_style, 0);

    // 大模型设置菜单项
    lv_ui.menu_main_page_label_llm = lv_label_create(lv_ui.menu_main_page_section_main);
    lv_label_set_text(lv_ui.menu_main_page_label_llm, LV_CUSTOM_SYMBOL_LLM" 智能体");
    lv_obj_add_style(lv_ui.menu_main_page_label_llm, &lv_ui.menu_item_style, 0);

    // 创建分隔符
    lv_ui.menu_main_page_seperator2 = lv_menu_separator_create(lv_ui.menu_main_page);

    // 创建关于另一个菜单项分区
    lv_ui.menu_main_page_section_about = lv_menu_section_create(lv_ui.menu_main_page);
    lv_obj_set_style_margin_hor(lv_ui.menu_main_page_section_about, 5, 0);
    lv_obj_set_style_pad_hor(lv_ui.menu_main_page_section_about, 5, 0);

    // 创建重置菜单项
    lv_ui.menu_main_page_label_reset = lv_label_create(lv_ui.menu_main_page_section_about);
    lv_label_set_text(lv_ui.menu_main_page_label_reset, LV_CUSTOM_SYMBOL_RESET" 恢复出厂设置");
    lv_obj_set_style_bg_color(lv_ui.menu_main_page_label_reset, lv_color_hex(0xAA0000), 0);
    lv_obj_add_style(lv_ui.menu_main_page_label_reset, &lv_ui.menu_item_style, 0);

    // 创建关于菜单项
    lv_ui.menu_main_page_label_about = lv_label_create(lv_ui.menu_main_page_section_about);
    lv_label_set_text(lv_ui.menu_main_page_label_about, LV_CUSTOM_SYMBOL_ABOUT" 关于");
    lv_obj_add_style(lv_ui.menu_main_page_label_about, &lv_ui.menu_item_style, 0);

    // 设置菜单返回按钮的图标为自定义的返回图标
    lv_image_set_src(lv_obj_get_child(lv_menu_get_main_header_back_button(lv_ui.menu), 0), LV_CUSTOM_SYMBOL_LEFT);

    setup_network_setting(); // 创建网络设置二级菜单页
    setup_settings_llm(); // 创建智能体设置二级菜单页
    setup_settings_reset(); // 创建重置菜单页
    setup_settings_about(); // 创建关于二级菜单页

    // 设置菜单默认显示设置页
    lv_menu_set_page(lv_ui.menu, lv_ui.menu_main_page);
    // 点击菜单事件（主要用于实现点击菜单根页面返回键时跳转到首页）
    lv_obj_add_event_cb(lv_ui.menu, menu_event_callback, LV_EVENT_CLICKED, NULL);

    // 为每一个菜单项设置跳转的二级菜单页
    lv_menu_set_load_page_event(lv_ui.menu, lv_ui.menu_main_page_label_network, lv_ui.menu_network_page);
    lv_menu_set_load_page_event(lv_ui.menu, lv_ui.menu_main_page_label_llm, lv_ui.menu_llm_page);
    lv_menu_set_load_page_event(lv_ui.menu, lv_ui.menu_main_page_label_about, lv_ui.menu_page_about);
    lv_menu_set_load_page_event(lv_ui.menu, lv_ui.menu_main_page_label_reset, lv_ui.menu_page_reset);
}
