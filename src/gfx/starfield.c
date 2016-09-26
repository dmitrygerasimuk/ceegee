/*
 * Copyright (C) 2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <allegro.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <xorshift.h>

#include "src/gfx/starfield.h"

// Number of visible stars.
#define NUM_STARS 700

// Number of hue shades.
int SHADES = 17;
// Offset at which the color shades begin.
int SHADES_OFFSET = 1;
// Number of luminance variants.
float LUMINANCES[] = { 1.0, 0.5, 0.25 };
int LUMINANCE_N = sizeof(LUMINANCES) / sizeof(float);
// Maximum rendering coordinates, and centers, for our stars.
int STAR_X_LIM = 320 - (((sizeof(LUMINANCES) / sizeof(float)) * 2) - 1);
int STAR_Y_LIM = 200 - (((sizeof(LUMINANCES) / sizeof(float)) * 2) - 1);
int STAR_X_C = (320 - (((sizeof(LUMINANCES) / sizeof(float)) * 2) - 1)) / 2;
int STAR_Y_C = (200 - (((sizeof(LUMINANCES) / sizeof(float)) * 2) - 1)) / 2;

// Speed at which the stars move, per vblank.
int STAR_SPEED = 1;

// Star definition. Contains a set of coordinates and a color value.
typedef struct star {
   int x, y, z;
} star;
// The visible universe.
star starfield[NUM_STARS];

/**
 * Draws a single star at x and y, using the LUMINANCE_N colors from offset c.
 * The color offset must be the first of the set.
 *
 * Uses _putpixel(), so use with care. Don't call with invalid x/y values.
 */
void draw_star(BITMAP *buffer, int x, int y, int c) {
    int a, b;
    int center = LUMINANCE_N - 1;

    // Center dot.
    _putpixel(buffer, center + x, center + y, palette_color[c]);

    // All other shades.
    for (a = 1; a < LUMINANCE_N; ++a) {
        _putpixel(buffer, center + x, center - a + y, palette_color[c + a]);
        _putpixel(buffer, center + x, center + a + y, palette_color[c + a]);
        _putpixel(buffer, center - a + x, center + y, palette_color[c + a]);
        _putpixel(buffer, center + a + x, center + y, palette_color[c + a]);
    }
}

/**
 * Returns the proper first color color index for a shade of hue.
 */
int star_hue_color(int hue) {
    return SHADES_OFFSET + (hue * 3);
}

/**
 * Determines and draws the positions and colors of the stars per frame.
 *
 * Stars can be either in a visible or invisible state. If visible, we
 * determine the next set of coordinates and render the star. If invisible,
 * we reset the star to a starting position somewhere in the center.
 */
void draw_starfield(BITMAP *buffer) {
    int a, sx, sy, sc;
    float hue;
    uint32_t seed_pos;

    for (a = 0; a < NUM_STARS; ++a) {
        if (starfield[a].z < 1) {
            // reset star
            seed_pos = xor32();
            starfield[a].x = (seed_pos % 96) - 48;
            starfield[a].y = ((seed_pos >> 16) % 96) - 48;
            starfield[a].z = 128 + (seed_pos % 256);
        }
        else {
            // move star
            starfield[a].z -= STAR_SPEED;
            if (starfield[a].z == 0) {
                continue;
            }
            sx = ((starfield[a].x * STAR_X_LIM) / starfield[a].z + (STAR_X_C));
            sy = ((starfield[a].y * STAR_Y_LIM) / starfield[a].z + (STAR_Y_C));
            hue = (starfield[a].z / 288.0);
            hue = hue < 1.0 ? hue : 1.0;
            sc = star_hue_color(hue * (SHADES - 1));
            if (sx > 0 && sx < STAR_X_LIM && sy > 0 && sy < STAR_Y_LIM) {
                draw_star(buffer, sx, sy, sc);
            } else {
                starfield[a].z = 0;
            }
        }
    }
}

/**
 * Sets all stars to the starting position. Their initial coordinates
 * are determined in the main loop.
 */
void initialize_starfield() {
    int a;
    for (a = 0; a < NUM_STARS; ++a) {
        starfield[a].z = 0;
    }
}

/**
 * Returns the palette used for the starfield.
 *
 * The colors are generated by picking SHADES number of hues
 * and adding LUMINANCE_N number of variants of that hue to the palette.
 */
RGB *get_starfield_palette() {
    int a, sha, lum, r, g, b, offset;
    RGB *pal;

    pal = malloc(sizeof(PALETTE));

    // Set all skipped colors to black.
    for (a = 0; a < SHADES_OFFSET; ++a) {
        pal[a].r = pal[a].g = pal[a].b = 0;
    }

    // Add several color swatches and their darker variants.
    for (sha = 0; sha < SHADES; ++sha) {
        offset = (sha * LUMINANCE_N) + SHADES_OFFSET;
        for (lum = 0; lum < LUMINANCE_N; ++lum) {
            hsv_to_rgb((360 / SHADES) * sha, 1.0, LUMINANCES[lum], &r, &g, &b);
            pal[offset + lum].r = r / 4;
            pal[offset + lum].g = g / 4;
            pal[offset + lum].b = b / 4;
        }
    }

    // Fill the rest of the palette with white.
    for (a = SHADES_OFFSET + (SHADES * LUMINANCE_N); a < PAL_SIZE; ++a) {
        pal[a].r = 63;
        pal[a].g = 63;
        pal[a].b = 63;
    }

    return pal;
}
