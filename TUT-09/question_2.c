/*
 *  CAB202 Tutorial 9: Debouncing, timers, & interrupts
 *	Question 2 - Template
 *
 *	B.Talbot, September 2015
 *	Queensland University of Technology
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"
#include "graphics.h"
#include "cpu_speed.h"

/**
 * This define controls whether button pins are initialised for the old or new
 * Teensy. You must change this based on which version you have!
 */
#define AM_I_OLD 0

/**
 * Useful defines you can use in your system time calculations
 */
#define FREQUENCY 8000000.0
#define PRESCALER 1024.0

/**
 * Function declarations
 */
void init_hardware(void);

void check_press_debounced(void);
double get_system_time(unsigned int timer_count);

/**
 * Main - initialise the hardware and run the code that blocks for button holds,
 * yet prints an up to date system time when not blocked.
 */
int main() {
    set_clock_speed(CPU_8MHz);

    // Setup the hardware
    init_hardware();

    // Draw the static information to the screen
    draw_string(27, 12, "TIMERS");
    draw_string(12, 20, "System time:");

    // Run the main loop
    char buff[80];
    float curr_time = 0.0;
    while (1) {
        // Get the current system time
        curr_time = get_system_time(TCNT1);

        // String-ise the current time, and draw it to the screen
        sprintf(buff, "%1.3f", curr_time);
        draw_string(35, 28, buff);

        // Show the screen and have a short nap
        show_screen();
        _delay_ms(100);
    }

    // We'll never get here...
    return 0;
}

/**
 * Function implementations
 */
void init_hardware(void) {
    // Initialising the LCD screen
    LCDInitialise(LCD_DEFAULT_CONTRAST);

    // Initialising the buttons as inputs (old and new)
    if (AM_I_OLD) {
        DDRB &= ~((1 << PB0) | (1 << PB1));
    } else {
        DDRF &= ~((1 << PF5) | (1 << PF6));
    }

    // Initialising the LEDs as outputs
    DDRB |= ((1 << PB2) | (1 << PB3));

    // Configure TIMER1 in "normal" operation mode
    TCCR1B &= ~((1<<WGM02));

    // Set the prescaler for TIMER1 so that the clock overflows every ~8.3 seconds
    TCCR1B |= ((1<<CS12) | (1<<CS10));
    TCCR1B &= ~((1<<CS11));
}

double get_system_time(unsigned int timer_count) {
    // Based on the current count, frequency, and prescaler - return the current
    // count time in seconds
    double scalar = 128 / 1000000.0;
    return (timer_count * scalar);
}
