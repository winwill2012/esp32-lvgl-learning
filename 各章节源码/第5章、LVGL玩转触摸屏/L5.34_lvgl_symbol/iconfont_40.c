/*******************************************************************************
 * Size: 40 px
 * Bpp: 2
 * Opts: --bpp 2 --size 40 --no-compress --stride 1 --align 1 --font iconfont.ttf --range 58938 --format lvgl -o iconfont_40.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef ICONFONT_40
#define ICONFONT_40 1
#endif

#if ICONFONT_40

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+E63A "" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1b,
    0xfe, 0x40, 0x0, 0x0, 0x2e, 0x40, 0x1b, 0x40,
    0x0, 0xf, 0x5, 0x55, 0xf, 0x0, 0x3, 0x89,
    0x0, 0x6, 0x2c, 0x0, 0xe2, 0x0, 0x0, 0x8,
    0xb0, 0x1c, 0x80, 0x0, 0x0, 0x23, 0x43, 0x0,
    0x0, 0x0, 0x0, 0x9c, 0x70, 0x0, 0x0, 0x0,
    0x4, 0xda, 0x0, 0x0, 0x0, 0x0, 0x1a, 0x90,
    0x0, 0x0, 0x0, 0x2, 0x6d, 0x0, 0x0, 0x0,
    0x0, 0x26, 0x90, 0x0, 0x0, 0x0, 0x2, 0x69,
    0x0, 0x0, 0x0, 0x0, 0x26, 0x60, 0x0, 0x0,
    0x0, 0x5, 0xd3, 0x0, 0x0, 0x0, 0x0, 0x8c,
    0x28, 0x0, 0x0, 0x0, 0x16, 0x80, 0xd0, 0x0,
    0x0, 0x0, 0x70, 0x3, 0x0, 0x0, 0x0, 0x5c,
    0x0, 0xd, 0x0, 0x0, 0xb, 0x0, 0x0, 0x30,
    0x0, 0x0, 0xc0, 0x0, 0x3, 0x0, 0x0, 0xc,
    0x0, 0x0, 0x30, 0x0, 0x0, 0xc0, 0x0, 0x3,
    0xff, 0xff, 0xfc, 0x0, 0x0, 0x34, 0x0, 0x1,
    0xc0, 0x0, 0x3, 0x0, 0x0, 0xc, 0x0, 0x0,
    0x30, 0x0, 0x0, 0xc0, 0x0, 0x3, 0x0, 0x0,
    0xc, 0x0, 0x0, 0x3f, 0xff, 0xff, 0xc0, 0x0,
    0x0, 0xa0, 0x0, 0xe0, 0x0, 0x0, 0x7, 0xff,
    0xfd, 0x0, 0x0, 0x0, 0x30, 0x0, 0xc0, 0x0,
    0x0, 0x3, 0xff, 0xfc, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 640, .box_w = 22, .box_h = 34, .ofs_x = 9, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 58938, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 2,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t iconfont_40 = {
#else
lv_font_t iconfont_40 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 34,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if ICONFONT_40*/
