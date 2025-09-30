#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    lv_obj_t* image = lv_image_create(lv_scr_act());
    lv_image_set_src(image, "A:/arduino-xiaozhi-course/L60_lvgl_filesystem_simulator/data/test.png");
    lv_obj_center(image);
}
