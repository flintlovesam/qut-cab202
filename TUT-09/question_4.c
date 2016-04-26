/*
 *  CAB202 Tutorial 9: Debouncing, timers, & interrupts
 *	Question 4 - Template
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
#include "sprite.h"

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
 * Definition of the smiley face sprite and it's x position
 */
#define BYTES_PER_FACE 32
unsigned char bm_happy[BYTES_PER_FACE] = {
		0b00000111, 0b11100000,
		0b00011000, 0b00011000,
		0b00100000, 0b00000100,
		0b01000000, 0b00000010,
		0b01011000, 0b00011010,
		0b10011000, 0b00011001,
		0b10000000, 0b00000001,
		0b10000000, 0b00000001,
		0b10010000, 0b00001001,
		0b10010000, 0b00001001,
		0b10001000, 0b00010001,
		0b01000111, 0b11100010,
		0b01000000, 0b00000010,
		0b00100000, 0b00000100,
		0b00011000, 0b00011000,
		0b00000111, 0b11100000
};
volatile float smiley_x = 0;     // There is something wrong with this... you will need to fix it!!!
volatile float curr_time = 0.0;
volatile int overflow_count = 0;
/**
 * Function implementations
 */
void init_hardware(void);

void check_press_debounced(void);
double get_system_time(unsigned int timer_count);

/**
 * Main - initialise the hardware, and run the code that increments the face's
 * current position and draws it.
 */
int main() {
	set_clock_speed(CPU_8MHz);

	// Create and initialise the sprite
	Sprite happy;
	init_sprite(&happy, 0, 16, 16, 16, bm_happy);

	// Setup the hardware
	init_hardware();

	// Run the main loop
	PORTB ^= (1 << PB2);
	int dx = 1;

	char buff[80];
	char buff2[20];

	while (1) {
		check_press_debounced();
		// Update the position and draw
		smiley_x += 1.0;
		happy.x = smiley_x;
		draw_sprite(&happy);
		curr_time = get_system_time(TCNT1); //this needs to move
		// Draw and rest between steps
		sprintf(buff, "%1.3f", curr_time);
		draw_string(58, 0, buff);
		_delay_ms(500);
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

	// Setup all necessary timers, and interrupts
	// TODO
	// Configure TIMER1 in "normal" operation mode
	TCCR1B &= ~((1<<WGM02));
	TCCR0B &= ~((1<<WGM02));

	// Set the prescaler for TIMER1 so that the clock overflows every ~8.3 seconds
	TCCR1B |= ((1<<CS12) | (1<<CS10));
	TCCR1B &= ~((1<<CS11));

	//Configure TIMER0 for counting to .5 seconds (every overflow, it counts.  at 15 overflows, it changes);
	TCCR0B |= ((1<<CS02) | (1<<CS00));
	TCCR0B &= ~((1<<CS01));
	//add to timer interrupt register
	TIMSK0 = (1<<TOIE0);
	sei();
}

void check_press_debounced(void) {
	// A button press should only be registered ONCE every time that the
	// button is RELEASED.
	int press_times = 0;
	while ((PINF>>6) & 1) {
		overflow_count = 0;
		press_times++;
	}
	if (press_times > 0) {
		press_times++;
	}
}

double get_system_time(unsigned int timer_count) {
	// Based on the current count, frequency, and prescaler - return the current
	// count time in seconds
	double scalar = 128 / 1000000.0;
	return (timer_count * scalar);
}

/**
 * Interrupt service routines
 */
ISR(TIMER0_OVF_vect){
		//increment the counter if interupted
		overflow_count++;
		//If count is over 15
		if (overflow_count > 15) {
			//Show what was already on the screen
			show_screen();
			//Stop changeing the LED's
			PORTB ^= 1 << PINB3;
			PORTB ^= 1 << PINB2;
			overflow_count = 0;
		}
}
