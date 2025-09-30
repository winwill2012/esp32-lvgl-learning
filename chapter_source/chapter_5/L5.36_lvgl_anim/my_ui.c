#include "my_ui.h"
#include "lvgl.h"

#define CIRCLE_SIZE 10
#define ANIM_TIME 600

void anim_exec_cb(void* obj, int32_t value)
{
    lv_obj_set_x(obj, value);
}

void show_ui()
{
    lv_color_t colors[4] = {
        lv_palette_main(LV_PALETTE_RED),
        lv_palette_main(LV_PALETTE_YELLOW),
        lv_palette_main(LV_PALETTE_GREEN),
        lv_palette_main(LV_PALETTE_BLUE)
    };

    int32_t center_x = lv_obj_get_width(lv_scr_act()) / 2;
    int32_t center_y = lv_obj_get_height(lv_scr_act()) / 2;

    lv_obj_t* circles[4];
    for (int i = 0; i < 4; i++)
    {
        circles[i] = lv_obj_create(lv_scr_act());
        lv_obj_set_size(circles[i], CIRCLE_SIZE, CIRCLE_SIZE);
        lv_obj_set_style_radius(circles[i], CIRCLE_SIZE / 2, 0);
        lv_obj_set_style_bg_color(circles[i], colors[i], 0);
        lv_obj_set_y(circles[i], center_y - CIRCLE_SIZE / 2);
        lv_obj_set_style_pad_all(circles[i], 0, 0);
        lv_obj_set_style_margin_all(circles[i], 0, 0);
        lv_obj_set_style_border_width(circles[i], 0, 0);
    }

    lv_obj_set_x(circles[0], center_x - 2 * CIRCLE_SIZE);
    lv_obj_set_x(circles[1], center_x - CIRCLE_SIZE);
    lv_obj_set_x(circles[2], center_x);
    lv_obj_set_x(circles[3], center_x + CIRCLE_SIZE);

    lv_anim_t anim1;
    lv_anim_init(&anim1);
    lv_anim_set_var(&anim1, circles[0]);
    lv_anim_set_values(&anim1, center_x - 2 * CIRCLE_SIZE, center_x - 3 * CIRCLE_SIZE);
    lv_anim_set_exec_cb(&anim1, anim_exec_cb);
    lv_anim_set_repeat_count(&anim1, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_duration(&anim1, ANIM_TIME);
    lv_anim_set_playback_duration(&anim1, ANIM_TIME);
    lv_anim_set_repeat_delay(&anim1, ANIM_TIME * 2);
    lv_anim_set_path_cb(&anim1, lv_anim_path_ease_in_out );

    lv_anim_t anim2;
    lv_anim_init(&anim2);
    lv_anim_set_var(&anim2, circles[3]);
    lv_anim_set_values(&anim2, center_x + CIRCLE_SIZE, center_x + 2 * CIRCLE_SIZE);
    lv_anim_set_exec_cb(&anim2, anim_exec_cb);
    lv_anim_set_repeat_count(&anim2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_duration(&anim2, ANIM_TIME);
    lv_anim_set_playback_duration(&anim2, ANIM_TIME);
    lv_anim_set_delay(&anim2, ANIM_TIME * 2);
    lv_anim_set_repeat_delay(&anim2, ANIM_TIME * 2);
    lv_anim_set_path_cb(&anim2, lv_anim_path_ease_in_out );

    lv_anim_start(&anim1);
    lv_anim_start(&anim2);
}
