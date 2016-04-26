/*
 *  CAB202 Tutorial 9: Debouncing, timers, & interrupts
 *	Question 3 - Template
 *
 *	B.Talbot, September 2015
 *	Queensland University of Technology
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

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
#define PRESCALER 256.0

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
    draw_string(17, 12, "INTERRUPTS");
    draw_string(12, 20, "System time:");

    // Run the main loop
    char buff[80];
    float curr_time;
    while (1) {
        // Call a debounced button checker
        check_press_debounced();

        // Get the current system time
        // TODO

        // String-ise the current time, and draw it to the screen
        sprintf(buff, "%5.4f", curr_time);
        draw_string(27, 28, buff);

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

     // Initalising the buttons as inputs (old and new)
     if (AM_I_OLD) {
         DDRB &= ~((1 << PB0) | (1 << PB1));
     } else {
         DDRF &= ~((1 << PF5) | (1 << PF6));
     }

     // Initialising the LEDs as outputs
     DDRB |= ((1 << PB2) | (1 << PB3));

     // Setup TIMER1in "normal" operation mode
     // TODO

     // Set the prescaler for TIMER1 so that the clock overflows every ~2.1 seconds
     // TODO

     // Enable the Timer Overflow Interrupt for TIMER1
     // TODO

     // Enable global interrupts
     // TODO

 }

void check_press_debounced(void) {
    // A button press should only be registered ONCE every time that the
    // button is RELEASED.
    // TODO
}

double get_system_time(unsigned int timer_count) {
    // Based on the current count, frequency, and prescaler - return the current
    // count time in seconds
    // TODO
    return 0.0;
}

/**
 * Interrupt service routines
 */
ISR(TIMER1_OVF_vect) {
    // Interrupt service routine for TIMER1. Toggle an LED everytime this ISR runs
    // TODO

}
