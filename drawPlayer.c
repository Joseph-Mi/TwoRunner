#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "background.c"

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