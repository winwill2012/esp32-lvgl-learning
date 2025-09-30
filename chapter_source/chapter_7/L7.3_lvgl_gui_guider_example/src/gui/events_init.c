/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static int32_t angle = 0;

static void home_turn_left_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        {
            // 要实现图片逆时针旋转90°
            lv_image_set_pivot(guider_ui.home_img_1, 120, 120);
            angle = (angle + 2700) % 3600;
            lv_image_set_rotation(guider_ui.home_img_1, angle);
            break;
        }
    default:
        break;
    }
}

static void home_turn_right_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        {
            // 要实现图片逆时针旋转90°
            lv_image_set_pivot(guider_ui.home_img_1, 120, 120);
            angle = (angle + 900) % 3600;
            lv_image_set_rotation(guider_ui.home_img_1, angle);
            break;
        }
    default:
        break;
    }
}

void events_init_home(lv_ui* ui)
{
    lv_obj_add_event_cb(ui->home_turn_left, home_turn_left_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_turn_right, home_turn_right_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui* ui)
{
}
