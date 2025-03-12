#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BACKGROUND_COLOR 0xFFFF
#define PIXEL_BASE 0xFF203020

volatile int *pixel = (int *)PIXEL_BASE;
int pixel_buffer_start;

void plot_pixel(int x, int y, short int line_color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return; //bound check
    volatile short int *pixel_address;
    pixel_address = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));
    *pixel_address = line_color;
}

void clear_screen() {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            plot_pixel(x, y, 0x0000); //Black
        }
    }
}

void draw_vert_line(int x0, int y0, int len, short int line_color) {
    int width = 7; // Fixed width for the vertical line

    for (int x = x0; x < x0 + width; x++) { // Loop over width (7 pixels)
        for (int y = y0; y < y0 + len; y++) { // Loop over length
            plot_pixel(x, y, line_color);
        }
    }
}

void draw_horiz_line(int x0, int y0, int len, short int line_color) {
    int width = 7; // Fixed width for the horizontal line

    for (int y = y0; y < y0 + width; y++) { // Loop over width (7 pixels)
        for (int x = x0; x < x0 + len; x++) { // Loop over length
            plot_pixel(x, y, line_color);
        }
    }
}

void draw_maze() {
    // Define wall color
    short int wall_color = 0xA534; // Brown-gray

    // Outer border
    draw_horiz_line(0, 0, SCREEN_WIDTH, wall_color);
    draw_horiz_line(0, SCREEN_HEIGHT - 7, SCREEN_WIDTH, wall_color);
    draw_vert_line(0, 0, SCREEN_HEIGHT, wall_color);
    draw_vert_line(SCREEN_WIDTH - 7, 0, SCREEN_HEIGHT, wall_color);

    // Internal maze structure with twists and turns
    draw_horiz_line(20, 20, 100, wall_color);
    draw_vert_line(120, 20, 80, wall_color);
    draw_horiz_line(40, 100, 80, wall_color);
    draw_vert_line(40, 100, 60, wall_color);
    draw_horiz_line(40, 160, 120, wall_color);
    draw_vert_line(160, 100, 60, wall_color);
    draw_horiz_line(160, 100, 100, wall_color);
    draw_vert_line(260, 100, 80, wall_color);
    draw_horiz_line(100, 180, 160, wall_color);
    draw_vert_line(100, 180, 40, wall_color);
    draw_horiz_line(100, 220, 180, wall_color);
}

int main(void) {
    pixel_buffer_start = *pixel;
    clear_screen();
    draw_player_1(200, 100);
    draw_player_2(10, 10);
    return 0;
}