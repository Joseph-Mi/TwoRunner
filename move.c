#include <stdlib.h>
#include <stddef.h>  // Defines NULL
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "drawPlayer.c"
	
#define PS2_BASE 0xFF200100  // Base address of PS/2 controller
#define N 2  // Changed to 2 players
	
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

void clear_screen(){
        int y, x;
        for (x = 0; x < 320; x++)
                for (y = 0; y < 240; y++)
                        plot_pixel1 (x, y, 0);
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

// void draw_player_1(int x0, int y0) {
//     int height = 5;
//     int width = 5;
//     for (int x = x0; x < x0 + width; x++) {
//         for (int y = y0; y < y0 + height; y++) {
//             if ((x == x0 + 1 || x == x0 + 3) && (y == y0 + 1)) {
//                 plot_pixel1(x, y, 0x0000); //Black (eyes)
//             } else if ((x == x0 + 1 || x == x0 + 2 || x == x0 + 3) && (y == y0 + 3)) {
//                 plot_pixel1(x, y, 0x0000); //Black (bottom line)
//             } else {
//                 plot_pixel1(x, y, 0xF7BB); //beige (body)
//             }
//         }
//     }
// }

// void draw_player_2(int x0, int y0) {
//     int height = 5;
//     int width = 5;
//     for (int x = x0; x < x0 + width; x++) {
//         for (int y = y0; y < y0 + height; y++) {
//             if ((x == x0 + 1 || x == x0 + 3) && (y == y0 + 1)) {
//                 plot_pixel1(x, y, 0x0000); //Black (eyes)
//             } else if ((x == x0 + 1 || x == x0 + 2 || x == x0 + 3) && (y == y0 + 3)) {
//                 plot_pixel1(x, y, 0x0000); //Black (bottom line)
//             } else {
//                 plot_pixel1(x, y, 0x07E0); //Green (body)
//             }
//         }
//     }
// }