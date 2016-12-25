/*
 * Copyright (C) 2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <allegro.h>
#include <stdio.h>

#include "src/audio/midi.h"
#include "src/game.h"
#include "src/gfx/deps/manager.h"
#include "src/gfx/deps/register.h"
#include "src/gfx/modes.h"
#include "src/gfx/res/flim.h"
#include "src/gfx/starfield/starfield.h"
#include "src/gfx/text.h"
#include "src/jukebox.h"
#include "src/utils/counters.h"

int REQ_ID_JUKEBOX_HANDLER;

// Track that's currently playing.
struct song *CURR_TRACK;

// How long (in ms) to wait when changing to the next track.
const int TRACK_CHANGE_WAIT = 1000;
// How long (in ms) between updates of the on-screen text.
const int UPDATE_FREQUENCY = 150;

// Help text.
char HELP_EXIT[] = "Press ESC to exit";
char HELP_ARROWS[] = "Use arrow keys to choose another song";

// Buffer we'll draw to before blitting to the screen.
BITMAP *buffer;

// Help text drawing information.
int font_height = 0;
int help_text_x = 0;
int help_text_y1 = 0;
int help_text_y2 = 0;
int help_text_y3 = 0;
int help_text_y4 = 0;

// Total length and number of beats (quarter notes) in the song.
int length, beats;
int track_n = 0;

/**
 * Draws two lines of help text on the screen to show the user
 * how to use the jukebox.
 */
void draw_help() {
    draw_text(buffer, help_text_x, help_text_y1, TXT_GRAY, -1, -1,
        TXT_REGULAR, TXT_CENTER, HELP_EXIT);
    draw_text(buffer, help_text_x, help_text_y2, TXT_GRAY, -1, -1,
        TXT_REGULAR, TXT_CENTER, HELP_ARROWS);
}

/**
 * Updates the display to show what song is currently playing.
 */
void update_song_data() {
    draw_textf(buffer, help_text_x, help_text_y3, TXT_WHITE, -1, -1,
        TXT_REGULAR, TXT_CENTER, "%d/%d: %s", track_n + 1,
        ALL_MUSIC_AMOUNT, CURR_TRACK->name);
}

/**
 * Draws the current time and duration of the song to the screen.
 */
void update_track_data() {
    draw_textf(buffer, help_text_x, help_text_y4, TXT_WHITE, -1, -1,
        TXT_REGULAR, TXT_CENTER, "%ld:%02ld/%d:%02d", midi_time / 60,
        midi_time % 60, length / 60, length % 60);
}

/**
 * Switches to graphics mode, installs Allegro drivers and initializes
 * variables used by the jukebox.
 */
void initialize_jukebox() {
    REQ_ID_JUKEBOX_HANDLER = req_id();

    // Register our game resources to the dependency manager.
    register_resources();

    // Switch to graphics mode.
    initialize_allegro();
    screen_gfx_mode();
    initialize_sound();
    dep_require(RES_ID_FLIM, REQ_ID_JUKEBOX_HANDLER);

    // Prepare for the jukebox loop.
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    RGB *pal = get_starfield_palette();
    add_text_colors(pal);
    set_palette(pal);
    free(pal);
    font_height = FLIM_HEIGHT;
    help_text_x = SCREEN_W / 2;
    help_text_y1 = SCREEN_H - (font_height * 5) - (font_height / 2);
    help_text_y2 = SCREEN_H - (font_height * 4) - (font_height / 2);
    help_text_y3 = SCREEN_H - (font_height * 3);
    help_text_y4 = SCREEN_H - (font_height * 2);
}

/**
 * The main loop of the jukebox. Waits for the end of the song (while updating
 * the timer) and then returns an indicator that we need the next song.
 * Also waits for keypresses; the user can press ESC, left arrow or right arrow
 * to trigger appropriate return codes.
 */
int jukebox_loop() {
    int key_p;
    while (TRUE) {
        while (keypressed()) {
            key_p = readkey() >> 8;
            if (key_p == KEY_ESC) {
                return JUKEBOX_EXIT;
            }
            if (key_p == KEY_LEFT) {
                return JUKEBOX_PREV_SONG;
            }
            if (key_p == KEY_RIGHT) {
                return JUKEBOX_NEXT_SONG;
            }
        }
        clear_bitmap(buffer);
        update_starfield(buffer);
        update_track_data();
        update_song_data();
        draw_help();
        vsync();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        // When we're at the end of the file, midi_pos will be set to
        // the negative number of beats in the song.
        if (midi_pos < 0) {
            rest(TRACK_CHANGE_WAIT);
            return JUKEBOX_NEXT_SONG;
        }
    }
}

/**
 * Runs the jukebox code. This contains its own game loop and can only be run
 * by adding the /j flag at startup.
 *
 * We'll display the song name, the current time and total time. When a song
 * is finished, we'll wait a second and then continue with the next.
 * Pressing ESC will quit the jukebox.
 */
int start_jukebox() {
    // Set up everything needed to run the jukebox.
    initialize_jukebox();
    initialize_starfield();

    MIDI *curr_music;
    int status;

    while (TRUE) {
        CURR_TRACK = ALL_MUSIC[track_n];

        // Prepare the screen.
        clear_to_color(buffer, palette_color[0]);

        // Load the next midi file.
        curr_music = load_midi(CURR_TRACK->file);
        length = get_midi_length(curr_music);
        // After calling get_midi_length(), midi_pos will contain the negative
        // number of beats, and midi_time the length of the midi in seconds.
        beats = -midi_pos;

        // Begin playback.
        play_midi(curr_music, FALSE);

        // Run the main loop; status is JUKEBOX_NEXT_SONG, JUKEBOX_PREV_SONG,
        // or JUKEBOX_EXIT if escape has been pressed.
        status = jukebox_loop();

        // Prepare for the next midi file.
        destroy_midi(curr_music);

        if (status == JUKEBOX_NEXT_SONG) {
            // Advance; loop back to zero if we're at the end.
            if (++track_n >= ALL_MUSIC_AMOUNT) {
                track_n = 0;
            }
        }
        else if (status == JUKEBOX_PREV_SONG) {
            // Go back one track.
            if (--track_n < 0) {
                track_n = ALL_MUSIC_AMOUNT - 1;
            }
        }
        else if (status == JUKEBOX_EXIT) {
            // Escape pressed; exit the jukebox.
            break;
        }
        else {
            // Unknown status.
            break;
        }
    }

    shutdown();
    return 0;
}
