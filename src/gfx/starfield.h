/*
 * Copyright (C) 2015-2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <allegro.h>
#include <inttypes.h>

#ifndef __CEEGEE_GFX_STARFIELD__
#define __CEEGEE_GFX_STARFIELD__

int loop_starfield(BITMAP *buffer);
int rand_star_color(uint32_t seed);
RGB *get_starfield_palette();
void draw_star(BITMAP *buffer, int x, int y, int c);
void draw_starfield(BITMAP *buffer);
void initialize_starfield();

#endif
