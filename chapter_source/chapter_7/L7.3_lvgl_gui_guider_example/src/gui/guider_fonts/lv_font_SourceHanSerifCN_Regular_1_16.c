/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --stride 1 --align 1 --font SourceHanSerifCN_Regular_1.otf --symbols 左 转右 --font iconfont.ttf --range 58881,58923 --format lvgl -o lv_font_SourceHanSerifCN_Regular_1_16.c
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



#ifndef LV_FONT_SOURCEHANSERIFCN_REGULAR_1_16
#define LV_FONT_SOURCEHANSERIFCN_REGULAR_1_16 1
#endif

#if LV_FONT_SOURCEHANSERIFCN_REGULAR_1_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+53F3 "右" */
    0x0, 0x0, 0x4, 0x0, 0x18, 0x0, 0x20, 0x2f,
    0xff, 0xe0, 0x80, 0x2, 0x0, 0x8, 0x4, 0x1f,
    0xfc, 0x50, 0x11, 0x20, 0x20, 0x40, 0x40, 0x80,
    0x81, 0xff, 0x2, 0x2, 0x0,

    /* U+5DE6 "左" */
    0x2, 0x0, 0xc, 0x0, 0x10, 0x17, 0xff, 0xf0,
    0x40, 0x0, 0x80, 0x2, 0x0, 0x7, 0xfc, 0x10,
    0x80, 0x21, 0x0, 0x82, 0x2, 0x4, 0x4, 0x8,
    0x53, 0xff, 0xc0,

    /* U+8F6C "转" */
    0x0, 0x60, 0x40, 0x80, 0x81, 0x7, 0xef, 0xe2,
    0x4, 0xa, 0x10, 0x94, 0xff, 0xaa, 0x40, 0xb8,
    0x88, 0x23, 0xf0, 0x48, 0x23, 0xe0, 0x89, 0xe,
    0x2, 0x6, 0x4, 0x4, 0x0, 0x0,

    /* U+E601 "" */
    0x2, 0x0, 0x68, 0xc, 0x80, 0x80, 0xb, 0xff,
    0xd0, 0x4, 0x80, 0x24, 0x1, 0x20, 0x9, 0x0,
    0x48, 0x2, 0x40, 0x13, 0xff, 0x80,

    /* U+E62B "" */
    0x3, 0x0, 0x1f, 0x0, 0x4c, 0x0, 0x2f, 0xfc,
    0xc0, 0x26, 0x1, 0x10, 0x8, 0x80, 0x44, 0x2,
    0x20, 0x11, 0xff, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 66, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 256, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 30, .adv_w = 256, .box_w = 15, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 57, .adv_w = 256, .box_w = 15, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 87, .adv_w = 256, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 256, .box_w = 13, .box_h = 12, .ofs_x = 2, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x53d3, 0x5dc6, 0x8f4c, 0xe5e1, 0xe60b
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 58892, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 6, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
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
    .bpp = 1,
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
const lv_font_t lv_font_SourceHanSerifCN_Regular_1_16 = {
#else
lv_font_t lv_font_SourceHanSerifCN_Regular_1_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_SOURCEHANSERIFCN_REGULAR_1_16*/
