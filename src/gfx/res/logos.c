/*
 * Copyright (C) 2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <allegro.h>
#include <stdio.h>

#include "src/gfx/res/logos.h"

DATAFILE *LOGOS;
int LOGOS_LOADED = FALSE;

// Startup images and their associated palettes.
int STARTUP_LOGO_BMP[] = {
    ASLOGO_IMG, TEST_IMG
};
int STARTUP_LOGO_PAL[] = {
    ASLOGO_PALETTE, TEST_PALETTE
};
int STARTUP_LOGOS = sizeof(STARTUP_LOGO_BMP) / sizeof(int);

/**
 * Load the logo images into memory.
 */
int load_logos_dat() {
    if (LOGOS_LOADED == TRUE) {
        return 0;
    }
    LOGOS = load_datafile(LOGOS_PATH);

    if (!LOGOS) {
        return 1;
    }

    LOGOS_LOADED = TRUE;
    return 0;
}

/**
 * Unload the logo images.
 */
void unload_logos_dat() {
    unload_datafile(LOGOS);
    LOGOS_LOADED = FALSE;
}
