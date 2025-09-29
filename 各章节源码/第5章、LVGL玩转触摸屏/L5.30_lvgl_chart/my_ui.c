#include "my_ui.h"
#include "lvgl.h"

static lv_chart_cursor_t* temp_cursor;
static lv_chart_cursor_t* hum_cursor;
static lv_chart_series_t* temp_series;
static lv_chart_series_t* hum_series;

void chart_event_callback(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t* chart = lv_event_get_target(e);
        uint32_t point = lv_chart_get_pressed_point(chart);
        lv_chart_set_cursor_point(chart, temp_cursor, temp_series, point);
        lv_chart_set_cursor_point(chart, hum_cursor, hum_series, point);
    }
}

void show_ui()
{
    lv_obj_t* chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, lv_pct(80), lv_pct(40));
    lv_obj_center(chart);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    temp_series = lv_chart_add_series(chart, lv_color_hex(0XFF0000), LV_CHART_AXIS_PRIMARY_Y);
    hum_series = lv_chart_add_series(chart, lv_color_hex(0X0000FF), LV_CHART_AXIS_SECONDARY_Y);

    lv_chart_set_div_line_count(chart, 5, 7);
    lv_chart_set_point_count(chart, 7);

    lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, "weekly temp & hum");
    lv_obj_align_to(title_label, chart, LV_ALIGN_OUT_TOP_MID, 0, -5);

    int32_t offset = lv_chart_get_first_point_center_offset(chart);

    lv_obj_t* scale = lv_scale_create(lv_scr_act());
    lv_obj_set_size(scale, lv_obj_get_width(chart), lv_pct(20));
    lv_scale_set_mode(scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_align_to(scale, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_scale_set_total_tick_count(scale, 7);
    lv_scale_set_major_tick_every(scale, 2);
    static const char* scale_text[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    lv_scale_set_text_src(scale, scale_text);
    lv_obj_set_style_pad_hor(scale, offset, 0);
    lv_obj_set_style_text_color(scale, lv_palette_main(LV_PALETTE_GREEN), 0);

    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 40);
    lv_obj_t* temp_begin_label = lv_label_create(lv_scr_act());
    lv_obj_t* temp_end_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_begin_label, "0");
    lv_label_set_text(temp_end_label, "40");
    lv_obj_align_to(temp_begin_label, chart, LV_ALIGN_OUT_LEFT_BOTTOM, 0, 0);
    lv_obj_align_to(temp_end_label, chart, LV_ALIGN_OUT_LEFT_TOP, 0, 0);

    lv_chart_set_axis_range(chart, LV_CHART_AXIS_SECONDARY_Y, 40, 90);
    lv_obj_t* hum_begin_label = lv_label_create(lv_scr_act());
    lv_obj_t* hum_end_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hum_begin_label, "40");
    lv_label_set_text(hum_end_label, "90");
    lv_obj_align_to(hum_begin_label, chart, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    lv_obj_align_to(hum_end_label, chart, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

    temp_cursor = lv_chart_add_cursor(chart, lv_palette_main(LV_PALETTE_RED), LV_DIR_LEFT);
    hum_cursor = lv_chart_add_cursor(chart, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_DIR_RIGHT);

    for (int i = 0; i < 7; i++)
    {
        lv_chart_set_next_value(chart, temp_series, (int32_t)lv_rand(10, 30));
        lv_chart_set_next_value(chart, hum_series, (int32_t)lv_rand(50, 80));
    }

    lv_obj_add_event_cb(chart, chart_event_callback, LV_EVENT_VALUE_CHANGED, NULL);
}
