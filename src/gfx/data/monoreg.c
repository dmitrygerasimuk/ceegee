/*
 * Copyright (C) 2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <allegro.h>
#include <stdio.h>

#include "monoreg.h"

DATAFILE *MONOREG;
int MONOREG_HEIGHT;
int MONOREG_LOADED = FALSE;

/**
 * Load the Monoreg font into memory.
 */
int load_monoreg() {
    if (MONOREG_LOADED == TRUE) {
        return 0;
    }
    MONOREG = load_datafile("data\\font\\monoreg.dat");

    if (!MONOREG) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        printf("Error loading the Monoreg font.");
        return 1;
    }

    // Acquire the font height, slightly adjusted.
    MONOREG_HEIGHT = text_height(MONOREG[MONOREG_COLOR].dat) - 4;

    MONOREG_LOADED = TRUE;
    return 0;
}

/**
 * Unload the Monoreg font. Unnecessary since it's used everywhere
 * in the game.
 */
int unload_monoreg() {
    unload_datafile(MONOREG);
}
