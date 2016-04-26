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

volatile int time_counter = 0;

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
		// Get the current timer
		curr_time = get_system_time(TCNT1);


		// String-ise the current time, and draw it to the screen
		sprintf(buff, "%5.3f", curr_time);
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

	// Initialising the LEDs as outputs
	DDRB |= ((1 << PB2) | (1 << PB3));

	//Turn on 1 led
	PORTB ^= 1 << PINB3;

	// Set the prescaler for TIMER1 so that the clock overflows every ~2.1 seconds
	TCCR1B &= ~((1<<WGM02));

	//prescaler of 256 (4 times less than 1024, overflows at 8.3/4 = 2.075)
	TCCR1B |= ((1<<CS02));
	TCCR1B &= ~((1<<CS01));

	// Enable the Timer Overflow Interrupt for TIMER1
	TIMSK1 = (1<<TOIE1);

	// Enable global interrupts
	sei();

}

double get_system_time(unsigned int timer_count) {
	// Based on the current count, frequency, and prescaler - return the current
	// count time in seconds
	double scalar = 128 / 4000000.0;
	return (timer_count * scalar);
}

/**
 * Interrupt service routines
 */
ISR(TIMER1_OVF_vect) {
	PORTB ^= 1 << PINB3;
	PORTB ^= 1 << PINB2;
}
