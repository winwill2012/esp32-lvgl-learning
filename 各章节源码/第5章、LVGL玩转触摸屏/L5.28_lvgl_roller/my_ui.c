#include "my_ui.h"
#include "lvgl.h"

void roller_event_callback(lv_event_t *e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        const lv_obj_t *roller = lv_event_get_target(e);
        // lv_roller_get_selected(roller);
        lv_roller_get_selected_str(roller, buf, sizeof(buf));
        LV_LOG_WARN("Clicked, ID: %d Text: %s\n", lv_roller_get_selected(roller), buf);
    }
}

void show_ui() {
    lv_obj_t *roller = lv_roller_create(lv_scr_act());
    lv_obj_set_width(roller, lv_display_get_horizontal_resolution(NULL));
    lv_obj_center(roller);

    // 设置选项
    lv_roller_set_options(roller, "Here With You\nYellow\nHello\nSmile\nDiamonds", LV_ROLLER_MODE_NORMAL);

    // 设置条目文字对齐方式为居中对齐
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // 设置默认被选中的条目的索引编号，从0开始
    // lv_roller_set_selected(roller, 2, LV_ANIM_ON);

    // 设置默认被选中的条目的文本
    lv_roller_set_selected_str(roller, "Yellow", LV_ANIM_OFF);

    // 设置显示多少个条目（会自动设置对应的高度）
    lv_roller_set_visible_row_count(roller, 3);

    lv_obj_add_event_cb(roller, roller_event_callback, LV_EVENT_ALL, NULL);
}
