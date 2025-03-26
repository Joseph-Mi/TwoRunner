#include <stdlib.h>
#include <stddef.h>  // Defines NULL
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
	
#define PS2_BASE 0xFF200100  // Base address of PS/2 controller
#define N 2  // Changed to 2 players
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BACKGROUND_COLOR 0xFFFF
	
volatile int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];
int x[N], y[N], dx[N], dy[N], x_fb[N], y_fb[N], speed_multiplier[N];
short int colour[N] = {0xf7e0, 0x07E0}; // Yellow and Green

void plot_pixel1(int x, int y, short int line_color);
void clear_screen();
void doEverything(int state, char key);
void erase(short int black);
void wait_for_vsync();
void initialize();
void draw_player_1(int x0, int y0);
void draw_player_2(int x0, int y0);

int main(void)
{
	initialize();	
	
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	/* set front pixel buffer to Buffer 1 */
	*(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the back buffer
	/* now, swap the front/back buffers, to set the front buffer location */
	wait_for_vsync();
	/* initialize a pointer to the pixel buffer, used by drawing functions */
	pixel_buffer_start = *pixel_ctrl_ptr;
	clear_screen(); // pixel_buffer_start points to the pixel buffer
	/* set back pixel buffer to Buffer 2 */
	*(pixel_ctrl_ptr + 1) = (int) &Buffer2;
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	clear_screen(); // pixel_buffer_start points to the pixel buffer
	
	/* Declare volatile pointers to I/O registers (volatile means that IO load
	and store instructions will be used to access these pointer locations,
	instead of regular memory loads and stores) */
	volatile int * PS2_ptr = (int *)PS2_BASE;
	int PS2_data, RVALID;
	char byte1 = 0, byte2 = 0, byte3 = 0;
	// PS/2 mouse needs to be reset (must be already plugged in)
	*(PS2_ptr) = 0xFF; // reset
	
	while (1){
		doEverything(0, 0); /* Erase any boxes and lines that were drawn in the last iteration */
		
		PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
		RVALID = PS2_data & 0x8000; // extract the RVALID field
		if (RVALID) {
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			if (byte2 == 0xF0){ //Read 0xF0 (key release) in last loop
				byte3 = NULL;
			}
		}
		
		doEverything(1, byte3);
		
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}
}

void plot_pixel1(int x, int y, short int line_color){
    volatile short int *one_pixel_address;
        one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1); //Address encodes x, y coordinates
        *one_pixel_address = line_color; //Store colour into location of address
}

void clear_screen() {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            plot_pixel(x, y, 0x0000); //Black
        }
    }
}

void doEverything(int state, char key){
	short int box_colour;
	int x0, y0, x1, y1;
	int next_pixel_color;

	

	for (int i = 0; i < N; i++){
		if (state == 0){ //erase
			box_colour = 0;
			x0 = x_fb[i];
			y0 = y_fb[i];
			// Clear previous position
			for (int dx = 0; dx < 5; dx++) {
				for (int dy = 0; dy < 5; dy++) {
					plot_pixel1(x0 + dx, y0 + dy, 0);
				}
			}
		}
		else{ //plot
			box_colour = colour[i];
			x0 = x[i];
			y0 = y[i];	
			
			// Draw player specific to their index
			if (i == 0) {
				draw_player_1(x0, y0);
			} else {
				draw_player_2(x0, y0);
			}
		}
		
		if (state == 0){ //store what's on screen (current front buffer)
			if (x_fb[i] == 2 && x[i] == 0){
				x_fb[i] = 1;
			}
			else if (x_fb[i] == 315 && x[i] == 317){
				x_fb[i] = 316;
			}
			else if (y_fb[i] == 2 && y[i] == 0){
				y_fb[i] = 1;
			}
			else if (y_fb[i] == 235 && y[i] == 237){
				y_fb[i] = 236;
			}
			else{
				x_fb[i] = x[i] - dx[i];
				y_fb[i] = y[i] - dy[i];
			}	
		}
		
		if (state == 1){ //update location based on key press
			// Player 1 (WASD) 
			if (i == 0) {
				if (key == 0x1D){ //W (move up)
					dx[i] = 0;
					dy[i] = -1;
					if (y[i] == 0){
						dy[i] = 0;
					}
				}
				else if (key == 0x1C){ //A (move left)
					dx[i] = -1;
					dy[i] = 0;
					if (x[i] == 0){
						dx[i] = 0;
					}
				}
				else if (key == 0x1B){ //S (move down)
					dx[i] = 0;
					dy[i] = 1;
					if (y[i] == 237){
						dy[i] = 0;
					}
				}
				else if (key == 0x23){ //D (move right)
					dx[i] = 1;
					dy[i] = 0;
					if (x[i] == 317){
						dx[i] = 0;
					}
				}
				else if (key == 0xF0 || key == NULL){ 
					dx[i] = 0;
					dy[i] = 0;
				}
			}
			// Player 2 (Arrow keys)
			else {
				if (key == 0x75){ //Up arrow
					dx[i] = 0;
					dy[i] = -1;
					if (y[i] == 0){
						dy[i] = 0;
					}
				}
				else if (key == 0x6B){ //Left arrow
					dx[i] = -1;
					dy[i] = 0;
					if (x[i] == 0){
						dx[i] = 0;
					}
				}
				else if (key == 0x72){ //Down arrow
					dx[i] = 0;
					dy[i] = 1;
					if (y[i] == 237){
						dy[i] = 0;
					}
				}
				else if (key == 0x74){ //Right arrow
					dx[i] = 1;
					dy[i] = 0;
					if (x[i] == 317){
						dx[i] = 0;
					}
				}
				else if (key == 0xF0 || key == NULL){ 
					dx[i] = 0;
					dy[i] = 0;
				}
			}
			
			x[i] += dx[i];
			y[i] += dy[i];
			
			if (x[i] >= 317){ //edge conditions
				x[i] = 317;
				dx[i] = 0;
			}
			else if (x[i] <= 0){
				x[i] = 0;
				dx[i] = 0;
			}
			if (y[i] >= 237){
				y[i] = 237;
				dy[i] = 0;
			}
			else if (y[i] <= 0){
				y[i] = 0;
				dy[i] = 0;
			}
		}
	}
}

void wait_for_vsync(){
	volatile int* fbuf = (int*) 0xFF203020;
	int status;
	*fbuf = 1; //store 1 into front buffer to initiate double buffering and buffer swap
	status = *(fbuf+3); //read status register
	while ((status & 0b1) != 0){ //poll until S bit is 0 (buffer swap & vsync finished)
		status = *(fbuf+3);
	}	
}

void initialize(){
	for (int i = 0; i < N; i++){
		x[i] = rand()%317; // 2x2 box, max x-value of top left corner of box cannot be border (319)
		y[i] = rand()%237; // 2x2 box, max y-value of top left corner of box cannot be border (239)
		dx[i] = 0;
		dy[i] = 0;
	}
}

void draw_player_1(int x0, int y0) {
    int height = 5;
    int width = 5;
    for (int x = x0; x < x0 + width; x++) {
        for (int y = y0; y < y0 + height; y++) {
            if ((x == x0 + 1 || x == x0 + 3) && (y == y0 + 1)) {
                plot_pixel1(x, y, 0x0000); //Black (eyes)
            } else if ((x == x0 + 1 || x == x0 + 2 || x == x0 + 3) && (y == y0 + 3)) {
                plot_pixel1(x, y, 0x0000); //Black (bottom line)
            } else {
                plot_pixel1(x, y, 0xF7BB); //beige (body)
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
                plot_pixel1(x, y, 0x0000); //Black (eyes)
            } else if ((x == x0 + 1 || x == x0 + 2 || x == x0 + 3) && (y == y0 + 3)) {
                plot_pixel1(x, y, 0x0000); //Black (bottom line)
            } else {
                plot_pixel1(x, y, 0x07E0); //Green (body)
            }
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
	draw_horiz_line(210, 34, 45, wall_color);
    draw_horiz_line(298, 34, 15, wall_color);

    draw_horiz_line(19, 49, 15, wall_color);        // Line 3 (top + 30)
    draw_horiz_line(109, 49, 30, wall_color);
	draw_horiz_line(298, 49, 15, wall_color);
	draw_horiz_line(196, 49, 15, wall_color);

    draw_horiz_line(7, 64, 45, wall_color);         // Line 4 (top + 45)
    draw_horiz_line(124, 64, 100, wall_color);
	draw_horiz_line(283, 64, 30, wall_color);

    draw_horiz_line(19, 79, 15, wall_color);       
    draw_horiz_line(64, 79, 30, wall_color);
	draw_horiz_line(152, 79, 59, wall_color);
	draw_horiz_line(223, 79, 18, wall_color);
	draw_horiz_line(283, 79, 15, wall_color);

    draw_horiz_line(52, 94, 103, wall_color);      // Line 6 (top + 75)
	draw_horiz_line(165, 94, 76, wall_color); 
	
    // Bottom lines (6 lines)
	draw_horiz_line(49, 218, 45, wall_color);
    draw_horiz_line(109, 218, 30, wall_color);    
	draw_horiz_line(154, 218, 12, wall_color);
	//draw_horiz_line(154, 218, 60, wall_color);
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
	
	draw_horiz_line(79, 173, 180, wall_color);
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
	draw_vert_line(79, 94, 53, wall_color);
	
	draw_vert_line(94, 19, 63, wall_color);
	draw_vert_line(94, 94, 34, wall_color);
	
	draw_vert_line(109, 0, 37, wall_color);
	draw_vert_line(109, 52, 30, wall_color);
	draw_vert_line(109, 110, 36, wall_color);
	draw_vert_line(109, 188, 15, wall_color);
	
	draw_vert_line(124, 49, 80, wall_color);
	draw_vert_line(124, 100, 20, wall_color);
	draw_vert_line(124, 158, 30, wall_color);
	
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

