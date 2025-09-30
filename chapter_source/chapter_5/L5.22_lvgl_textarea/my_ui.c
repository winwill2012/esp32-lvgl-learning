#include "my_ui.h"
#include "lvgl.h"


static lv_obj_t *keyboard;

void textarea_event_callback(lv_event_t* e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    // 输入框获得焦点或者被点击
    if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED)
    {
        // 显示键盘
        lv_obj_remove_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    } else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY || code == LV_EVENT_LEAVE)  // 输入框失去焦点，或者按下键盘上的OK按键
    {
        // 隐藏键盘
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}
void show_ui()
{
    lv_obj_t* ta = lv_textarea_create(lv_scr_act());
    lv_obj_set_size(ta, 200, 40);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 10, 20);

    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, LV_SYMBOL_CALL);
    lv_obj_align_to(label, ta, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    // 设置为密码模式
    lv_textarea_set_password_mode(ta, false);

    // 设置单行模式
    lv_textarea_set_one_line(ta, true);
    // 设置输入框可以输入的字符集合
    lv_textarea_set_accepted_chars(ta, "1234567890-");
    // 设置输入框最大输入字符数
    lv_textarea_set_max_length(ta, 13);
    // 设置输入框输入提示
    lv_textarea_set_placeholder_text(ta, "phone number");

    // 创建一个键盘
    keyboard = lv_keyboard_create(lv_scr_act());
    // 设置键盘模式为数字模式（其余还有大写模式、小写模式、字符模式）
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
    // 将键盘与输入框进行绑定
    lv_keyboard_set_textarea(keyboard, ta);
    // 开始时隐藏键盘
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    // 为输入框添加事件响应函数
    lv_obj_add_event_cb(ta, textarea_event_callback, LV_EVENT_ALL, NULL);
}
