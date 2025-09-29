#include "my_ui.h"
#include "lvgl.h"
#include "lv_spiffs.h"

void show_ui()
{
    if (!lv_init_spiffs())
    {
        return;
    }
    auto image = lv_image_create(lv_scr_act());
    lv_obj_center(image);
    lv_image_set_src(image, "S:/test.png");
}
