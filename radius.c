#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BACKGROUND_COLOR 0xFFFF
#define PIXEL_BASE 0xFF203020

// #include "background.c"

volatile int *pixel = (int *)PIXEL_BASE;
int pixel_buffer_start;

void create_sphere(int x, int y, int x2, int y2, int radius1, int radius2) {
   for (int i = 0; i < SCREEN_WIDTH; i++) {
      for (int j = 0; j < SCREEN_HEIGHT; j++) {
         if ((i - x) * (i - x) + (j - y) * (j - y) > radius1 * radius1 && (i - x2) * (i - x2) + (j - y2) * (j - y2) > radius2 * radius2) {
            plot_pixel(i, j, 0x0000);
         }
      }
   }
}

// comment out later
void plot_pixel(int x, int y, short int line_color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return; //bound check
    volatile short int *pixel_address;
    pixel_address = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));
    *pixel_address = line_color;
}

//20-25 without boost and 35 with boost
