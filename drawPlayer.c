#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BACKGROUND_COLOR 0xFFFF
#define PIXEL_BASE 0xFF203020

#include "background.c"

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
    // Top lines (6 lines)
    draw_horiz_line(19, 19, 84, wall_color);        // Line 1 (top)
    draw_horiz_line(146, 19, 103, wall_color);
    draw_horiz_line(261, 19, 31, wall_color);

    draw_horiz_line(0, 34, 59, wall_color);         // Line 2 (top + 15)
    draw_horiz_line(71, 34, 45, wall_color);
    draw_horiz_line(134, 34, 31, wall_color);
    draw_horiz_line(177, 34, 160, wall_color);

    draw_horiz_line(19, 49, 31, wall_color);        // Line 3 (top + 30)
    draw_horiz_line(19, 49, 31, wall_color);

    draw_horiz_line(1, 64, 59, wall_color);         // Line 4 (top + 45)
    draw_horiz_line(120, 64, 100, wall_color);

    draw_horiz_line(100, 79, 40, wall_color);       // Line 5 (top + 60)
    draw_horiz_line(100, 79, 40, wall_color);

    draw_horiz_line(100, 94, 180, wall_color);      // Line 6 (top + 75)
	
	
    // Bottom lines (6 lines)
    draw_horiz_line(180, 218, 180, wall_color);     
	draw_horiz_line(10, 218, 10, wall_color);
	
    draw_horiz_line(10, 203, 80, wall_color);
	draw_horiz_line(100, 203, 180, wall_color);
	
    draw_horiz_line(10, 188, 30, wall_color);
	draw_horiz_line(90, 188, 80, wall_color);
	
    draw_horiz_line(30, 173, 80, wall_color);
	draw_horiz_line(100, 173, 180, wall_color);
	
    draw_horiz_line(100, 158, 180, wall_color); 
	draw_horiz_line(10, 158, 80, wall_color);
	
    draw_horiz_line(100, 143, 180, wall_color); 
	draw_horiz_line(30, 143, 70, wall_color); 

	
	
    // Vertical 
	//1
    draw_vert_line(19, 0, 80, wall_color);
	draw_vert_line(19, 100, 80, wall_color);
	
    draw_vert_line(34, 0, 80, wall_color);
	draw_vert_line(34, 100, 80, wall_color);
	
	draw_vert_line(49, 0, 80, wall_color);
	draw_vert_line(49, 100, 80, wall_color);
	
	draw_vert_line(64, 0, 80, wall_color);
	draw_vert_line(64, 100, 80, wall_color);
	
	draw_vert_line(79, 0, 80, wall_color);
	draw_vert_line(79, 100, 80, wall_color);
	
	draw_vert_line(94, 0, 80, wall_color);
	draw_vert_line(94, 100, 80, wall_color);
	
	draw_vert_line(109, 0, 80, wall_color);
	draw_vert_line(109, 100, 80, wall_color);
	
	draw_vert_line(124, 0, 80, wall_color);
	draw_vert_line(124, 100, 80, wall_color);
	
	//2
	draw_vert_line(298, 0, 80, wall_color);
	draw_vert_line(298, 100, 80, wall_color);
	
	draw_vert_line(283, 0, 80, wall_color);
	draw_vert_line(283, 100, 80, wall_color);
	
	draw_vert_line(268, 0, 80, wall_color);
	draw_vert_line(268, 100, 80, wall_color);
	
	draw_vert_line(253, 0, 80, wall_color);
	draw_vert_line(253, 100, 80, wall_color);
	
	draw_vert_line(238, 0, 80, wall_color);
	draw_vert_line(238, 100, 80, wall_color);
	
	draw_vert_line(223, 0, 80, wall_color);
	draw_vert_line(223, 100, 80, wall_color);
	
	draw_vert_line(208, 0, 80, wall_color);
	draw_vert_line(208, 100, 80, wall_color);
	
	draw_vert_line(193, 0, 80, wall_color);
	draw_vert_line(193, 100, 80, wall_color);
	
	/////////////////MID BOX//////////////////
	//outer
	draw_horiz_line(132, 102, 24, wall_color);
	draw_horiz_line(163, 102, 24, wall_color);
	draw_horiz_line(132, 135, 24, wall_color);
	draw_horiz_line(163, 135, 24, wall_color);
	draw_vert_line(132, 102, 33, wall_color);
	draw_vert_line(184, 102, 33, wall_color);
	
	//inner
	draw_horiz_line(142, 110, 35, wall_color);
	draw_horiz_line(142, 125, 35, wall_color);
	draw_vert_line(142, 110, 18, wall_color);
	draw_vert_line(174, 110, 5, wall_color);
	draw_vert_line(174, 123, 5, wall_color);
}


int main(void) {
    pixel_buffer_start = *pixel;
    clear_screen();

    short int wall_color = 0xF6900; //earth
    short int outline_color = 0xF6900; //earth

    draw_outline(outline_color); // Draw thicker outline
    draw_maze(wall_color); // Draw maze walls
	
	//draw_player_1(190,224);

    return 0;
}
