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
    int width = 3; // Fixed width for the vertical line

    for (int x = x0; x < x0 + width; x++) { // Loop over width (7 pixels)
        for (int y = y0; y < y0 + len; y++) { // Loop over length
            plot_pixel(x, y, line_color);
        }
    }
}

void draw_horiz_line(int x0, int y0, int len, short int line_color) {
    int width = 3; // Fixed width for the horizontal line

    for (int y = y0; y < y0 + width; y++) { // Loop over width (7 pixels)
        for (int x = x0; x < x0 + len; x++) { // Loop over length
            plot_pixel(x, y, line_color);
        }
    }
}

void draw_thick_horiz_line(int x0, int y0, int len, short int color, int width) {
    for (int y = y0; y < y0 + width; y++) {
        for (int x = x0; x < x0 + len; x++) {
            plot_pixel(x, y, color);
        }
    }
}

void draw_thick_vert_line(int x0, int y0, int len, short int color, int width) {
    for (int x = x0; x < x0 + width; x++) {
        for (int y = y0; y < y0 + len; y++) {
            plot_pixel(x, y, color);
        }
    }
}

void draw_outline(short int color) {
    int border_width = 7; // Thicker border
    draw_thick_horiz_line(0, 0, SCREEN_WIDTH, color, border_width);
    draw_thick_horiz_line(0, SCREEN_HEIGHT - border_width, SCREEN_WIDTH, color, border_width);
    draw_thick_vert_line(0, 0, SCREEN_HEIGHT, color, border_width);
    draw_thick_vert_line(SCREEN_WIDTH - border_width, 0, SCREEN_HEIGHT, color, border_width);
}

void draw_maze(short int wall_color) {
    //h
    //1
    draw_horiz_line(19, 19, 84, wall_color);
    draw_horiz_line(146, 19, 103, wall_color);
    draw_horiz_line(261, 19, 31, wall_color);

    draw_horiz_line(0, 34, 59, wall_color);
    draw_horiz_line(71, 34, 45, wall_color);
    draw_horiz_line(134, 34, 31, wall_color);
    draw_horiz_line(177, 34, 160, wall_color);
    
    draw_horiz_line(19, 49, 31, wall_color);
    draw_horiz_line(19, 49, 31, wall_color);
    //2
    draw_horiz_line(100, 64, 40, wall_color);
    draw_horiz_line(100, 79, 40, wall_color);
    
    //draw_inner_box();
    draw_horiz_line(100, 114, 40, wall_color);
    draw_horiz_line(100, 119, 40, wall_color);
    
    draw_horiz_line(100, 138, 40, wall_color);
    draw_horiz_line(100, 157, 40, wall_color);
    
    //3
    draw_horiz_line(100, 176, 40, wall_color);
	
    draw_horiz_line(100, 195, 40, wall_color);
    draw_horiz_line(100, 214, 180, wall_color);
    
    //v
    //1
    draw_vert_line(115, 0, 80, wall_color);
    
    draw_vert_line(40, 100, 60, wall_color);
    
    //2
    
    //3
    draw_vert_line(100, 261, 40, wall_color);
    draw_vert_line(100, 276, 40, wall_color);
    draw_vert_line(100, 191, 40, wall_color);
    draw_vert_line(100, 206, 40, wall_color);
    draw_vert_line(100, 221, 40, wall_color); //why is this not 240-19-7
}

int main(void) {
    pixel_buffer_start = *pixel;
    clear_screen();

    short int wall_color = 0xF6900; //earth
    short int outline_color = 0xF6900; //earth

    draw_outline(outline_color); // Draw thicker outline
    draw_maze(wall_color); // Draw maze walls

    return 0;
}
