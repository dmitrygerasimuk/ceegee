/*
 * Copyright (C) 2015-2017, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <allegro.h>
#include <stdio.h>

#include "src/gfx/text.h"
#include "src/gfx/deps/manager.h"
#include "src/gfx/res/flim.h"
#include "src/gfx/res/tin.h"

DATAFILE *font_data;

/**
 * Adds the standard font colors to a palette. They're added to the
 * end of the palette, at positions 252-254 (255 being reserved for black).
 * These colors are used by the Flim and Tin fonts.
 */
void add_text_colors(RGB *pal) {
    pal[254].r = pal[254].g = pal[254].b = 63;
    pal[253].r = pal[253].g = pal[253].b = 28;
    pal[252].r = pal[252].g = pal[252].b = 12;
}

/**
 * Draws text onto a buffer, with string formatting. The formatted
 * string is passed to draw_text() with all other arguments verbatim.
 */
void draw_textf(BITMAP *buffer, int x, int y, int color_a, int color_b,
    int bg, int font, int align, const char *format, ...)
{
    char txt[TXT_MAX_SIZE];
    va_list ap;

    // String formatting.
    va_start(ap, format);
    uvszprintf(txt, sizeof(txt), format, ap);
    va_end(ap);

    draw_text(buffer, x, y, color_a, color_b, bg, font, align, txt);
}

/**
 * Draws text onto a buffer using our standardized fonts and settings.
 *
 * This is an abstraction around Allegro's text drawing routines.
 * It allows you to set most options that are normally available as well,
 * and uses our custom fonts. Two colors can be passed, although
 * to get the standardized colors you should pass TXT_WHITE or another
 * label to color_a (in that event, color_b will be ignored).
 */
void draw_text(BITMAP *buffer, int x, int y, int color_a, int color_b,
    int bg, int font, int align, char txt[TXT_MAX_SIZE])
{
    void (*txt_fn)() = 0;

    // Assign the text drawing function based on alignment.
    switch (align) {
        case TXT_LEFT:
            txt_fn = textout_ex;
            break;
        case TXT_CENTER:
            txt_fn = textout_centre_ex;
            break;
        case TXT_RIGHT:
            txt_fn = textout_right_ex;
            break;
    }

    // Set text color; either the desired colors were passed directly,
    // or color_a was set to e.g. TXT_WHITE. If it's the latter,
    // set the true colors here.
    // Note, all predefined colors have a value of less than zero.
    if (color_a < 0) {
        switch (color_a) {
            case TXT_WHITE:
                color_a = palette_color[254];
                color_b = palette_color[253];
                break;
            case TXT_GRAY:
                color_a = palette_color[253];
                color_b = palette_color[252];
                break;
        }
    }

    // Finally, print the text using a custom setup for each font.
    switch (font) {
        case TXT_REGULAR:
            font_data = dep_data_ref(RES_ID_FLIM);
            txt_fn(buffer, font_data[FLIM_WHITE].dat, txt, x, y, color_a, bg);
            txt_fn(buffer, font_data[FLIM_GRAY].dat, txt, x, y, color_b, bg);
            break;
        case TXT_SMALL:
            font_data = dep_data_ref(RES_ID_TIN);
            txt_fn(buffer, font_data[TIN_WHITE].dat, txt, x, y, color_a, bg);
            txt_fn(buffer, font_data[TIN_GRAY].dat, txt, x, y, color_b, bg);
            break;
    }
}
