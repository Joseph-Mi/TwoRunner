#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

volatile int *pixel = (int *)0xFF203020;
int pixel_buffer_start;

void plot_pixel(int x, int y, short int line_color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return; //bound check
    volatile short int *pixel_address;
    pixel_address = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));
    *pixel_address = line_color;
}

void draw_blue_block_with_branches() {
    // Calculate center of the screen
    int center_x = SCREEN_WIDTH / 2;
    int center_y = SCREEN_HEIGHT / 2;
    
    // Pure blue color
    short int blue_color = 0x001F;
    
    // Draw the 2x2 center block
    for (int x = center_x - 1; x < center_x + 1; x++) {
        for (int y = center_y - 1; y < center_y + 1; y++) {
            plot_pixel(x, y, blue_color);
        }
    }
    
    plot_pixel(center_x - 2, center_y - 2, blue_color);
    plot_pixel(center_x + 1, center_y - 2, blue_color);
    plot_pixel(center_x - 2, center_y + 1, blue_color);
    plot_pixel(center_x + 1, center_y + 1, blue_color);
}

int main(void) {
    pixel_buffer_start = *pixel;
    clear_screen();

    draw_blue_block_with_branches();

    return 0;
}