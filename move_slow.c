#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// #include "drawPlayer.c"

#define PS2_BASE 0xFF200100
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

volatile int pixel_buffer_start;
short int Buffer1[240][512];
short int Buffer2[240][512];

// Player 1 variables (renamed)
int player1_x, player1_y, player1_dx, player1_dy;
// Player 2 variables (renamed)
int player2_x, player2_y, player2_dx, player2_dy;

int speed_multiplier = 1;

void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void update_players(char key);
void draw_player_1(int x0, int y0);
void draw_player_2(int x0, int y0);
void wait_for_vsync();
void initialize();

int main(void)
{
    initialize();    

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1;
    wait_for_vsync();
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen();
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    clear_screen();

    volatile int * PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    char byte1 = 0, byte2 = 0, byte3 = 0;
    *(PS2_ptr) = 0xFF; // reset

    while (1) {
        clear_screen();

        PS2_data = *(PS2_ptr);
        RVALID = PS2_data & 0x8000;
        if (RVALID) {
            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data & 0xFF;

            if (byte2 == 0xE0) {  // Extended key
                update_players(byte3);
            } else if (byte2 != 0xF0) {  // Not a key release
                update_players(byte3);
            }
        }

        // Update positions
        player1_x += player1_dx;
        player1_y += player1_dy;
        player2_x += player2_dx;
        player2_y += player2_dy;

        // Boundary checks
        player1_x = (player1_x < 0) ? 0 : (player1_x > SCREEN_WIDTH - 5) ? SCREEN_WIDTH - 5 : player1_x;
        player1_y = (player1_y < 0) ? 0 : (player1_y > SCREEN_HEIGHT - 5) ? SCREEN_HEIGHT - 5 : player1_y;
        player2_x = (player2_x < 0) ? 0 : (player2_x > SCREEN_WIDTH - 5) ? SCREEN_WIDTH - 5 : player2_x;
        player2_y = (player2_y < 0) ? 0 : (player2_y > SCREEN_HEIGHT - 5) ? SCREEN_HEIGHT - 5 : player2_y;

        // Draw players
        draw_player_1(player1_x, player1_y);
        draw_player_2(player2_x, player2_y);

        wait_for_vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    }
}

void plot_pixel(int x, int y, short int line_color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    volatile short int *pixel_address;
    pixel_address = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));
    *pixel_address = line_color;
}

void clear_screen() {
    for (int x = 0; x < SCREEN_WIDTH; x++)
        for (int y = 0; y < SCREEN_HEIGHT; y++)
            plot_pixel(x, y, 0);
}

void update_players(char key) {
    // Player 1 controls (WASD)
    if (key == 0x1D) { player1_dx = 0; player1_dy = -speed_multiplier; }  // W
    else if (key == 0x1C) { player1_dx = -speed_multiplier; player1_dy = 0; }  // A
    else if (key == 0x1B) { player1_dx = 0; player1_dy = speed_multiplier; }  // S
    else if (key == 0x23) { player1_dx = speed_multiplier; player1_dy = 0; }  // D

    // Player 2 controls (Arrow keys)
    else if (key == 0x75) { player2_dx = 0; player2_dy = -speed_multiplier; }  // Up
    else if (key == 0x6B) { player2_dx = -speed_multiplier; player2_dy = 0; }  // Left
    else if (key == 0x72) { player2_dx = 0; player2_dy = speed_multiplier; }  // Down
    else if (key == 0x74) { player2_dx = speed_multiplier; player2_dy = 0; }  // Right
}

void wait_for_vsync() {
    volatile int* fbuf = (int*) 0xFF203020;
    *fbuf = 1;
    while ((*((volatile int *)0xFF20302C) & 0x1) != 0);
}

void initialize() {
    // Randomly initialize player positions
    player1_x = rand() % (SCREEN_WIDTH - 5);
    player1_y = rand() % (SCREEN_HEIGHT - 5);
    player2_x = rand() % (SCREEN_WIDTH - 5);
    player2_y = rand() % (SCREEN_HEIGHT - 5);
    player1_dx = player1_dy = player2_dx = player2_dy = 0;
}

void draw_player_1(int x0, int y0) {
    int height = 5;
    int width = 5;
    for (int x = x0; x < x0 + width; x++) {
        for (int y = y0; y < y0 + height; y++) {
            if ((x == x0 + 1 || x == x0 + 3) && (y == y0 + 1)) {
                plot_pixel(x, y, 0x0000); //Black (eyes)
            } else if ((x == x0 + 1 || x == x0 + 2 || x == x0 + 3) && (y == y0 + 3)) {
                plot_pixel(x, y, 0x0000); //Black (bottom line)
            } else {
                plot_pixel(x, y, 0xF7BB); //beige (body)
            }
        }
    }
}

void draw_player_2(int x0, int y0) {
    int height = 5;
    int width = 5;
    for (int x = x0; x < x0 + width; x++) {
        for (int y = y0; y < y0 + height; y++) {
            if ((x == x0 + 1 || x == x0 + 3) && (y == y0 + 1)) {
                plot_pixel(x, y, 0x0000); //Black (eyes)
            } else if ((x == x0 + 1 || x == x0 + 2 || x == x0 + 3) && (y == y0 + 3)) {
                plot_pixel(x, y, 0x0000); //Black (bottom line)
            } else {
                plot_pixel(x, y, 0x07E0); //Green (body)
            }
        }
    }
}