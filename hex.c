#include <stdint.h>

// Define hardware addresses
#define HEX_BASE1 0xFF200020
#define HEX_BASE2 0xFF200030
#define TIMER_BASE 0xFF202000
#define KEY_BASE 0xFF200050
#define TIMER_VAL 1250  // 0.5s at 50 MHz (25M cycles for 1s, 12.5M for 0.5s)

// Volatile pointer definitions for memory-mapped I/O
volatile uint32_t* const hex_display1 = (uint32_t*)HEX_BASE1;  // For HEX0-HEX3
volatile uint32_t* const hex_display2 = (uint32_t*)HEX_BASE2;  // For HEX4-HEX5
volatile uint32_t* const timer = (uint32_t*)TIMER_BASE;
volatile uint32_t* const key = (uint32_t*)KEY_BASE;

// Letter patterns for GAME OVER
const uint8_t G = 0x3D;  // G
const uint8_t A = 0x77;  // A
const uint8_t M = 0x37;  // M (approximation)
const uint8_t E = 0x79;  // E
const uint8_t O = 0x3F;  // O
const uint8_t V = 0x3E;  // V(U) (approximation)
const uint8_t R = 0x77;  // r (approximation)
const uint8_t BLANK = 0x00;  // Blank

// Define the message as an array with blank spaces before and after GAME
const uint8_t GAME_OVER[] = {BLANK, G, A, M, E, BLANK, O, V, E, R, BLANK};
const int MESSAGE_LENGTH = 11;

// Function to write a digit to a specific HEX display
void write_hex_digit(int digit, int display_num) {
    uint32_t bit_pattern = digit;
    uint32_t shift_amount = (display_num % 4) * 8;
    
    if (display_num < 4) {
        // HEX0-HEX3
        uint32_t current_value = *hex_display1;
        uint32_t mask = ~(0xFF << shift_amount);
        *hex_display1 = (current_value & mask) | (bit_pattern << shift_amount);
    } else {
        // HEX4-HEX5
        uint32_t current_value = *hex_display2;
        uint32_t mask = ~(0xFF << shift_amount);
        *hex_display2 = (current_value & mask) | (bit_pattern << shift_amount);
    }
}

// Function to clear all displays
void clear_all_displays() {
    *hex_display1 = 0;
    *hex_display2 = 0;
}

// Function to configure and start timer with 0.5s delay
void start_timer() {
    // Set timer period
    timer[2] = (TIMER_VAL >> 16) & 0xFFFF;  // High half
    timer[3] = TIMER_VAL & 0xFFFF;          // Low half
    
    // Start timer: CONT=1, START=1, ITO=1 (bit pattern 0x7)
    timer[1] = 0x7;
}

// Function to wait for timer timeout
void wait_for_timer() {
    // Wait until TO bit is set
    while ((timer[0] & 0x1) == 0) {
        // Wait
    }
    
    // Clear the timeout flag
    timer[0] = 0;
}

int main() {
    // Clear displays initially
    clear_all_displays();
    
    // Configure timer
    start_timer();
    
    // Define a window of 6 characters to be displayed
    int window_start = 0;
    
    // Main loop to scroll the message
    while (1) {
        // Clear displays each cycle
        clear_all_displays();
        
        // Display the current window of characters
        for (int i = 0; i < 6; i++) {
            // Calculate which character to display at this position
            int message_index = (window_start + i) % MESSAGE_LENGTH;
            
            // Display the character - HEX5 is leftmost, HEX0 is rightmost
            write_hex_digit(GAME_OVER[message_index], 5 - i);
        }
        
        // Wait for 0.5 seconds
        wait_for_timer();
        
        // Move window to the left (towards HEX0)
        window_start = (window_start + 1) % MESSAGE_LENGTH;
    }
    
    return 0;  // Never reached
}