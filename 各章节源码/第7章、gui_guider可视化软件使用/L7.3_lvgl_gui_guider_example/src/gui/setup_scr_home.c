/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

#define LV_CUSTOMER_SYMBOL_TURN_LEFT            "\xEE\x98\x81"
#define LV_CUSTOMER_SYMBOL_TURN_RIGHT           "\xEE\x98\xAB"

void setup_scr_home(lv_ui *ui)
{
    //Write codes home
    ui->home = lv_obj_create(NULL);
    lv_obj_set_size(ui->home, 240, 320);
    lv_obj_set_scrollbar_mode(ui->home, LV_SCROLLBAR_MODE_OFF);

    //Write style for home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_1
    ui->home_img_1 = lv_image_create(ui->home);
    lv_obj_set_pos(ui->home_img_1, 0, 0);
    lv_obj_set_size(ui->home_img_1, 240, 240);
    lv_obj_add_flag(ui->home_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->home_img_1, &_IMG_202508025489_240x240_RGB565A8_240x240);
    lv_image_set_pivot(ui->home_img_1, 50,50);
    lv_image_set_rotation(ui->home_img_1, 0);

    //Write style for home_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->home_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->home_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_turn_left
    ui->home_turn_left = lv_button_create(ui->home);
    lv_obj_set_pos(ui->home_turn_left, 29, 258);
    lv_obj_set_size(ui->home_turn_left, 80, 40);
    ui->home_turn_left_label = lv_label_create(ui->home_turn_left);
    lv_label_set_text(ui->home_turn_left_label, "" LV_CUSTOMER_SYMBOL_TURN_LEFT " 左转");
    lv_label_set_long_mode(ui->home_turn_left_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_turn_left_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_turn_left, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->home_turn_left_label, LV_PCT(100));

    //Write style for home_turn_left, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_turn_left, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_turn_left, lv_color_hex(0xe67796), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_turn_left, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_turn_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_turn_left, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_turn_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_turn_left, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_turn_left, &lv_font_SourceHanSerifCN_Regular_1_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_turn_left, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_turn_left, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_turn_right
    ui->home_turn_right = lv_button_create(ui->home);
    lv_obj_set_pos(ui->home_turn_right, 127, 258);
    lv_obj_set_size(ui->home_turn_right, 80, 40);
    ui->home_turn_right_label = lv_label_create(ui->home_turn_right);
    lv_label_set_text(ui->home_turn_right_label, "" LV_CUSTOMER_SYMBOL_TURN_RIGHT " 右转");
    lv_label_set_long_mode(ui->home_turn_right_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_turn_right_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_turn_right, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->home_turn_right_label, LV_PCT(100));

    //Write style for home_turn_right, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_turn_right, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_turn_right, lv_color_hex(0x61a9f3), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_turn_right, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_turn_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_turn_right, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_turn_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_turn_right, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_turn_right, &lv_font_SourceHanSerifCN_Regular_1_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_turn_right, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_turn_right, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of home.


    //Update current screen layout.
    lv_obj_update_layout(ui->home);

    //Init events for screen.
    events_init_home(ui);
}
