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
    // Top lines (6 lines)
    draw_horiz_line(19, 19, 30, wall_color);        // Line 1 (top)
    draw_horiz_line(124, 19, 42, wall_color);
	draw_horiz_line(195, 19, 15, wall_color);
    draw_horiz_line(269, 19, 30, wall_color);

    draw_horiz_line(0, 34, 52, wall_color);         // Line 2 (top + 15)
    draw_horiz_line(94, 34, 15, wall_color);
	draw_horiz_line(124, 34, 30, wall_color);
	draw_horiz_line(210, 34, 15, wall_color);
	draw_horiz_line(240, 34, 15, wall_color);
    draw_horiz_line(298, 34, 15, wall_color);

    draw_horiz_line(19, 49, 15, wall_color);        // Line 3 (top + 30)
    draw_horiz_line(109, 49, 30, wall_color);
	draw_horiz_line(298, 49, 15, wall_color);
	draw_horiz_line(196, 49, 15, wall_color);

    draw_horiz_line(7, 64, 45, wall_color);         // Line 4 (top + 45)
    draw_horiz_line(124, 64, 72, wall_color);
	draw_horiz_line(208, 64, 15, wall_color);
	draw_horiz_line(283, 64, 30, wall_color);

    draw_horiz_line(19, 79, 15, wall_color);       
    draw_horiz_line(64, 79, 30, wall_color);
	draw_horiz_line(152, 79, 59, wall_color);
	draw_horiz_line(223, 79, 18, wall_color);
	draw_horiz_line(283, 79, 15, wall_color);

    draw_horiz_line(64, 94, 91, wall_color);      // Line 6 (top + 75)
	draw_horiz_line(165, 94, 76, wall_color); 
	
    // Bottom lines (6 lines)
	draw_horiz_line(49, 218, 45, wall_color);
    draw_horiz_line(109, 218, 30, wall_color);    
	draw_horiz_line(154, 218, 12, wall_color);
	draw_horiz_line(194, 218, 45, wall_color);
	
    draw_horiz_line(7, 203, 30, wall_color);
	draw_horiz_line(79, 203, 60, wall_color);
	draw_horiz_line(194, 203, 45, wall_color);
	draw_horiz_line(268, 203, 30, wall_color);
	
    draw_horiz_line(7, 188, 30, wall_color);
	draw_horiz_line(64, 188, 18, wall_color);
	draw_horiz_line(94, 188, 15, wall_color);
	draw_horiz_line(179, 188, 45, wall_color);
	draw_horiz_line(253, 188, 18, wall_color);
	draw_horiz_line(283, 188, 18, wall_color);
	
	draw_horiz_line(79, 173, 117, wall_color);
	draw_horiz_line(208, 173, 60, wall_color);
	draw_horiz_line(34, 173, 30, wall_color);
	
    draw_horiz_line(109, 158, 73, wall_color); 
	draw_horiz_line(19, 158, 78, wall_color);
	draw_horiz_line(208, 158, 32, wall_color); 
	draw_horiz_line(253, 158, 30, wall_color); 
	 
	draw_horiz_line(0, 143, 37, wall_color); 
	draw_horiz_line(223, 143, 46, wall_color); 
	draw_horiz_line(94, 143, 45, wall_color);
	draw_horiz_line(152, 143, 43, wall_color);

	
    // Vertical 
	//left
    draw_vert_line(19, 37, 15, wall_color);
	draw_vert_line(19, 82, 45, wall_color);
	draw_vert_line(19, 173, 15, wall_color);
	draw_vert_line(19, 203, 15, wall_color);
	
    draw_vert_line(34, 67, 15, wall_color);
	draw_vert_line(34, 100, 45, wall_color);
	draw_vert_line(34, 160, 15, wall_color);
	draw_vert_line(34, 220, 15, wall_color);
	
	draw_vert_line(49, 19, 15, wall_color);
	draw_vert_line(49, 49, 60, wall_color);
	draw_vert_line(49, 125, 22, wall_color);
	draw_vert_line(49, 188, 30, wall_color);
	
	draw_vert_line(64, 19, 47, wall_color);
	draw_vert_line(64, 143, 15, wall_color);
	draw_vert_line(64, 173, 45, wall_color);
	
	draw_vert_line(79, 7, 60, wall_color);
	draw_vert_line(79, 110, 36, wall_color);
	draw_vert_line(79, 160, 15, wall_color);
	
	draw_vert_line(94, 19, 63, wall_color);
	draw_vert_line(94, 94, 34, wall_color);
	
	draw_vert_line(109, 0, 37, wall_color);
	draw_vert_line(109, 52, 30, wall_color);
	draw_vert_line(109, 110, 36, wall_color);
	draw_vert_line(109, 188, 15, wall_color);
	
	draw_vert_line(124, 49, 80, wall_color);
	draw_vert_line(124, 100, 20, wall_color);
	draw_vert_line(124, 158, 33, wall_color);
	
	//right
	draw_vert_line(298, 19, 15, wall_color);
	draw_vert_line(298, 79, 67, wall_color);
	draw_vert_line(298, 158, 33, wall_color);
	draw_vert_line(298, 203, 80, wall_color);
	
	draw_vert_line(283, 22, 30, wall_color);
	draw_vert_line(283, 94, 97, wall_color);
	draw_vert_line(283, 100, 80, wall_color);
	draw_vert_line(283, 218, 15, wall_color);
	
	draw_vert_line(268, 19, 60, wall_color);
	draw_vert_line(268, 94, 34, wall_color);
	draw_vert_line(268, 143, 15, wall_color);
	draw_vert_line(268, 173, 15, wall_color);
	draw_vert_line(268, 203, 18, wall_color);
	
	draw_vert_line(253, 0, 68, wall_color);
	draw_vert_line(253, 79, 65, wall_color);
	draw_vert_line(253, 203, 90, wall_color);
	
	draw_vert_line(238, 19, 48, wall_color);
	draw_vert_line(238, 109, 19, wall_color);
	draw_vert_line(238, 176, 45, wall_color);
	
	draw_vert_line(223, 19, 18, wall_color);
	draw_vert_line(223, 109, 35, wall_color);
	draw_vert_line(223, 30, 51, wall_color);
	
	draw_vert_line(208, 7, 15, wall_color);
	draw_vert_line(208, 96, 51, wall_color);
	draw_vert_line(208, 160, 15, wall_color);
	draw_vert_line(208, 49, 15, wall_color);
	draw_vert_line(208, 49, 15, wall_color);
	
	draw_vert_line(193, 0, 22, wall_color);
	draw_vert_line(193, 40, 12, wall_color);
	draw_vert_line(193, 109, 52, wall_color);
	draw_vert_line(193, 34, 18, wall_color);
	draw_vert_line(193, 188, 18, wall_color);
	
	/////////////////MID BOX//////////////////
	//outer
	draw_horiz_line(132, 102, 23, wall_color);
	draw_horiz_line(165, 102, 22, wall_color);
	draw_horiz_line(132, 135, 24, wall_color);
	draw_horiz_line(164, 135, 23, wall_color);
	draw_vert_line(132, 102, 33, wall_color);
	draw_vert_line(184, 102, 33, wall_color);
	
	//inner
	draw_horiz_line(142, 110, 35, wall_color);
	draw_horiz_line(142, 125, 35, wall_color);
	draw_vert_line(142, 110, 18, wall_color);
	draw_vert_line(174, 110, 5, wall_color);
	draw_vert_line(174, 123, 5, wall_color);
	
	//horiz
	draw_horiz_line(64, 110, 15, wall_color);
	draw_horiz_line(240, 109, 15, wall_color);
	draw_horiz_line(34, 125, 45, wall_color);
	draw_horiz_line(300, 125, 20, wall_color);
	draw_horiz_line(270, 125, 16, wall_color);
	
	//vert
	draw_vert_line(138, 79, 18, wall_color);
	draw_vert_line(152, 19, 18, wall_color);
	draw_vert_line(165, 0, 22, wall_color);
	draw_vert_line(179, 19, 45, wall_color);
	
	draw_vert_line(152, 49, 18, wall_color);
	draw_vert_line(152, 95, 8, wall_color);
	draw_vert_line(165, 95, 8, wall_color);
	draw_vert_line(165, 20, 32, wall_color);
	draw_vert_line(165, 174, 17, wall_color);
	draw_vert_line(165, 65, 17, wall_color);
	
	draw_vert_line(138, 173, 18, wall_color);
	draw_vert_line(138, 203, 30, wall_color);
	draw_vert_line(152, 188, 33, wall_color);
	draw_vert_line(179, 203, 30, wall_color);
	draw_vert_line(165, 203, 30, wall_color);
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
