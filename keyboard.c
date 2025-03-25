#include <stdio.h>
#include <stdlib.h>

#include "drawPlayer.c"

#define PS2_BASE 0xFF200108

volatile int *PS2_ptr = (int *) PS2_BASE;

// Player positions
int player1_x = 10, player1_y = 10;
int player2_x = 20, player2_y = 10;

// Function to move player based on scancode input
void move_player(char scancode) {
    switch (scancode) {
        // Player 1 (WASD)
        case 0x1D: player1_y--; break; // W
        case 0x1B: player1_y++; break; // S
        case 0x1C: player1_x--; break; // A
        case 0x23: player1_x++; break; // D

        // Player 2 (Arrow Keys)
        case 0x75: player2_y--; break; // Up
        case 0x72: player2_y++; break; // Down
        case 0x6B: player2_x--; break; // Left
        case 0x74: player2_x++; break; // Right

        default: return;
    }

    // Redraw players with updated positions
    drawPlayer(1, player1_x, player1_y);
    drawPlayer(2, player2_x, player2_y);
}

// Main function to read PS/2 input and update players
int main(void) {
    int PS2_data, RVALID;
    char byte1 = 0, byte2 = 0, byte3 = 0;

    *(PS2_ptr) = 0xFF; // Reset PS/2 device

    while (1) {
        PS2_data = *(PS2_ptr); // Read the PS/2 data register
        RVALID = PS2_data & 0x8000; // Check if data is valid

        if (RVALID) {
            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data & 0xFF; // Extract scancode

            move_player(byte3); // Move player based on input
        }
    }

    return 0;
}
