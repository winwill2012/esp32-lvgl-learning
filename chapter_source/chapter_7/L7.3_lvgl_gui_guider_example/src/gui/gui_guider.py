# Copyright 2025 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import utime as time
import usys as sys
import lvgl as lv
import ustruct
import fs_driver

lv.init()

# Register display driver.
disp_drv = lv.sdl_window_create(240, 320)
lv.sdl_window_set_resizeable(disp_drv, False)
lv.sdl_window_set_title(disp_drv, "Simulator (MicroPython)")

# Regsiter input driver
mouse = lv.sdl_mouse_create()

# Add default theme for bottom layer
bottom_layer = lv.layer_bottom()
lv.theme_apply(bottom_layer)

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_scale(v)

def anim_img_rotate_cb(obj, v):
    obj.set_rotation(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.binfont_create(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.image_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = lv.calendar.__cast__(e.get_current_target())
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RESULT.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    datetext = lv.label.__cast__(e.get_target())
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = lv.screen_active()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = datetext.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, datetext), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    calendar = lv.calendar.__cast__(e.get_current_target())
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if calendar.get_pressed_date(date) == lv.RESULT.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.remove_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            calendar.delete()

# Create home
home = lv.obj()
home.set_size(240, 320)
home.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for home, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
home.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
home.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
home.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create home_img_1
home_img_1 = lv.image(home)
home_img_1.set_src(load_image(r"F:\gui_guider\gui_guider_demo2\generated\MicroPython\IMG_202508025489_240x240_240_240.png"))
home_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
home_img_1.set_pivot(50,50)
home_img_1.set_rotation(0)
home_img_1.set_pos(0, 0)
home_img_1.set_size(240, 240)
# Set style for home_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
home_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create home_turn_left
home_turn_left = lv.button(home)
home_turn_left_label = lv.label(home_turn_left)
home_turn_left_label.set_text(""+lv.SYMBOL.LOOP+" 左转")
home_turn_left_label.set_long_mode(lv.label.LONG.WRAP)
home_turn_left_label.set_width(lv.pct(100))
home_turn_left_label.align(lv.ALIGN.CENTER, 0, 0)
home_turn_left.set_style_pad_all(0, lv.STATE.DEFAULT)
home_turn_left.set_pos(29, 258)
home_turn_left.set_size(80, 40)
# Set style for home_turn_left, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
home_turn_left.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_bg_color(lv.color_hex(0xe67796), lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_text_font(test_font("SourceHanSerifCN_Regular_1", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_left.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create home_turn_right
home_turn_right = lv.button(home)
home_turn_right_label = lv.label(home_turn_right)
home_turn_right_label.set_text(""+lv.SYMBOL.LOOP+" 右转")
home_turn_right_label.set_long_mode(lv.label.LONG.WRAP)
home_turn_right_label.set_width(lv.pct(100))
home_turn_right_label.align(lv.ALIGN.CENTER, 0, 0)
home_turn_right.set_style_pad_all(0, lv.STATE.DEFAULT)
home_turn_right.set_pos(127, 258)
home_turn_right.set_size(80, 40)
# Set style for home_turn_right, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
home_turn_right.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_bg_color(lv.color_hex(0x61a9f3), lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_text_font(test_font("SourceHanSerifCN_Regular_1", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
home_turn_right.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

home.update_layout()

def home_turn_left_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

home_turn_left.add_event_cb(lambda e: home_turn_left_event_handler(e), lv.EVENT.ALL, None)

def home_turn_right_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

home_turn_right.add_event_cb(lambda e: home_turn_right_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.screen_load(home)

if __name__ == '__main__':
    while True:
        lv.task_handler()
        time.sleep_ms(5)
