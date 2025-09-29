#include "my_ui.h"
#include "lvgl.h"

void show_ui()
{
    auto qrcode = lv_qrcode_create(lv_scr_act());
    lv_obj_center(qrcode);
    lv_qrcode_set_size(qrcode, 50);
    const char* url = "https://www.baidu.com";
    lv_qrcode_update(qrcode, url, lv_strlen(url));
}
