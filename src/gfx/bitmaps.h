/*
 * Copyright (C) 2015-2017, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <stdbool.h>

#ifndef __CEEGEE_GFX_BITMAPS__
#define __CEEGEE_GFX_BITMAPS__

void fade_in_bitmap_file(char *file);
void fade_in_bitmap(BITMAP *image, RGB *pal, bool unload);
void fade_out_bitmap();
void show_bitmaps();

#endif
