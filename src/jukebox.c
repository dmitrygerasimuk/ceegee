/*
 * Copyright (C) 2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#include <stdio.h>

#include "src/audio/midi.h"
#include "src/game.h"
#include "src/jukebox.h"

// Track that's currently playing.
char *CURR_TRACK;

// How long (in ms) to wait when changing to the next track.
int TRACK_CHANGE_WAIT = 1000;
// How long (in ms) between updates of the on-screen text.
int UPDATE_FREQUENCY = 250;

// Help text.
char HELP_EXIT[] = "Press ESC to exit";
char HELP_ARROWS[] = "Use arrow keys to choose another song";

// Text colors and background color.
int song_color, help_color, background_color;
// Height of our font in pixels.
int font_height = 0;
// Current track number.
int track_n = 0;
// Total length and number of beats (quarter notes) in the song.
int length, beats;

/**
 * Draws two lines of help text on the screen to show the user
 * how to use the jukebox.
 */
void draw_help() {
    textprintf_centre_ex(
        screen, font, SCREEN_W / 2, SCREEN_H - (font_height * 5) -
        (font_height / 2), help_color, -1, HELP_EXIT
    );
    textprintf_centre_ex(
        screen, font, SCREEN_W / 2, SCREEN_H - (font_height * 4) -
        (font_height / 2), help_color, -1, HELP_ARROWS
    );
}

/**
 * Updates the display to show what song is currently playing.
 * This function is called each time we switch to a new song.
 */
void update_song_data() {
    textprintf_centre_ex(
        screen, font, SCREEN_W / 2, SCREEN_H - (font_height * 3), song_color,
        -1, "%d/%d: %s", track_n + 1, ALL_MUSIC_AMOUNT, CURR_TRACK
    );
}

/**
 * Draws the current time and duration of the song to the screen.
 */
void update_track_data() {
    textprintf_centre_ex(
        screen, font, SCREEN_W / 2, SCREEN_H - (font_height * 2), song_color,
        background_color, "%d:%02d / %d:%02d", midi_time / 60, midi_time % 60,
        length / 60, length % 60
    );
}

/**
 * Switches to graphics mode, installs Allegro drivers and initializes
 * variables used by the jukebox.
 */
void initialize_jukebox() {
    // Switch to graphics mode.
    initialize();
    screen_gfx_mode();
    initialize_sound();

    // Prepare for the jukebox loop.
    set_palette(desktop_palette);
    font_height = text_height(font);
    song_color = makecol(255, 255, 255);
    help_color = makecol(128, 128, 128);
    background_color = makecol(0, 0, 0);
}

/**
 * The main loop of the jukebox. Waits for the end of the song (while updating
 * the timer) and then returns an indicator that we need the next song.
 * Also waits for keypresses; the user can press ESC, left arrow or right arrow
 * to trigger appropriate return codes.
 */
int jukebox_loop() {
    int key;
    while (TRUE) {
        while (keypressed()) {
            key = readkey() >> 8;
            if (key == KEY_ESC) {
                return JUKEBOX_EXIT;
            }
            if (key == KEY_LEFT) {
                return JUKEBOX_PREV_SONG;
            }
            if (key == KEY_RIGHT) {
                return JUKEBOX_NEXT_SONG;
            }
        }
        update_track_data();
        rest(UPDATE_FREQUENCY);

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

    MIDI *curr_music;
    int status;

    while (TRUE) {
        CURR_TRACK = ALL_MUSIC[track_n];

        // Prepare the screen.
        clear_to_color(screen, background_color);
        draw_help();
        update_song_data();

        // Load the next midi file.
        curr_music = load_midi(CURR_TRACK);
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
