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
    int width = 7;

    for (int x = x0; x < x0 + width; x++) {
        
    }
}

void draw_horiz_line(int x0, int y0, int len, short int line_color) {
    int width = 7;


}

void draw_maze() {
    int x0 = 0;
    int y0 = 0;
    int x1 = 320;
    int y1 = 240;
    draw_line(x0, y0, x1, y0, 0x0000); //Top
    draw_line(x0, y0, x0, y1, 0x0000); //Left
    draw_line(x1, y0, x1, y1, 0x0000); //Right
    draw_line(x0, y1, x1, y1, 0x0000); //Bottom
    draw_line(0, 0, 320, 240, 0x0000); //Diagonal
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

int main(void) {
    pixel_buffer_start = *pixel;
    clear_screen();
    draw_player_1(200, 100);
    draw_player_2(10, 10);
    return 0;
}