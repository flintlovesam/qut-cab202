//--------------------------------------------------
// CAB202 - Assignment 2 - Falling Faces
//--------------------------------------------------
// Author: Tylor Stewart - n9013555
// Tutor: Falouthy Bahfenne
// Due Date: 18/10/2015 @ 11:59pm
//--------------------------------------------------

// Import required libraries.
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "graphics.h"
#include "cpu_speed.h"
#include "sprite.h"

#include "usb_serial.h"

// Define constants.


// Global variables.
int lives = 3;
int score = 0;
int level = 0;

int debug = 0;
int randSeed = 0;
char buff[20];
int playerSprite = 0;

float speed[3] = {1, 2, 3};
int speedMod = 0;

// Sprite Bitmaps.
unsigned char bmAngry[8] = {0b01111110, 0b10000001, 0b10100101, 0b10000001, 0b10011001, 0b10100101, 0b10000001, 0b01111110};
unsigned char bmSmiley[8] = {0b01111110, 0b10000001, 0b10100101, 0b10000001, 0b10100101, 0b10011001, 0b10000001, 0b01111110};
unsigned char bmCrazy[8] = {0b01111110, 0b10000101, 0b10100001, 0b10000001, 0b10111101, 0b10001001, 0b10000001, 0b01111110};
unsigned char bmPlayer[2][8] = {
	{0b00011000, 0b00111100, 0b01111110, 0b11011011, 0b11111111, 0b01011010, 0b10000001, 0b01000010},
	{0b00011000, 0b00111100, 0b01111110, 0b11011011, 0b11111111, 0b00100100, 0b01011010, 0b10100101}
};

// Cerate some Sprites.
Sprite angry;
Sprite smiley;
Sprite crazy;
Sprite player;

// Function implementations.
void initHardware(void);
void fallingFaces();
void setupGame(int);
void drawBorder();
void drawBanner(int lives, int score, int level);
int selectLevel();
void startGame();
void spawnPlayer(int level);
void spawnFaces(int level);
void moveFaces();
void collisionDetection();
void winLoose();
uint16_t adc_read(uint8_t ch);

// Helper functions.
void drawCentred(unsigned char y, char* string);
void debugPrint();
int potPosition();
void sendLine(char* string);

// Main - Start the Falling Faces game.
int main() {
	// Setup the Hardware.
	initHardware();
	
	// Display Name and Student Number.
	drawCentred(15, "Tylor Stewart");
	drawCentred(25, "n9013555");
	
	show_screen();
	(debug) ? _delay_ms(250) : _delay_ms(3000);
	clear_screen();
	
	// Start the game...
	fallingFaces();

	// We'll never get here!
	return 0;
}


//--------------------------------------------------
// Function Implimentations
//--------------------------------------------------
void initHardware(void) {
    // Set the clock-speed.
    set_clock_speed(CPU_8MHz);
    
    // Initialize the LCD screen.
    LCDInitialise(LCD_DEFAULT_CONTRAST);
	
	// Initialising SW1 as input.
	DDRB &= ~((1 << PB1) | (1 << PB7));
	DDRD &= ~((1 << PD0) | (1 << PD1));

    // Initialising SW2 and SW3 as inputs.
    DDRF &= ~((1 << PF5) | (1 << PF6));

    // Initialising LED0 and LED1 as outputs.
    DDRB |= ((1 << PB2) | (1 << PB3));
 
    // Initialising ADC with a pre-scaler of 128.
	ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	// Setup TIMER0 and add it to the interrupt register.
	TCCR0B |= ((1 << CS02) | (1 << CS00));
	TIMSK0 = (1<<TOIE0);
	
	// Enable interrupts globally.
	sei();
	
	// Initialise the USB serial
    usb_init();
}

void fallingFaces() {
	// Level Selection.
	level = selectLevel();
	
	// Setup game for selected level.
	setupGame(level);
	
	// Start the game...
	startGame();
}

int selectLevel() {
	int level = 1;			// Placeholder for Level Selection.
	while (!((PINF>>PF6) & 1)) {
	    clear_screen();		// Clear the screen.
		
		drawCentred(0, "Select a level!");
	    
	    // Display the options for the three levels.
	    (level == 1) ? drawCentred(10, "> Level 1 <") : drawCentred(10, "Level 1");
	    (level == 2) ? drawCentred(20, "> Level 2 <") : drawCentred(20, "Level 2");
	    (level == 3) ? drawCentred(30, "> Level 3 <") : drawCentred(30, "Level 3");
	    
	    // Simple Debounce to check SW3 - Incrament/Loop the level selection.
	    int press = 0; while ((PINF>>PF5) & 1) press++; int inc = (press > 0) ? 1 : 0;
	    (inc) ? (level == 3) ? level = 1 : level++ : 0;
	    
	    show_screen();		// Show the screen.
		
		randSeed++;
	}
	
	// Serial connection for Level 3.
	if (level == 3) {
		clear_screen();
		drawCentred(15, "Please Connect");
    	drawCentred(25, "Serial Terminal...");
    	show_screen();
		
		// Wait for the serial connection to be initiated by the player.
    	while(!usb_configured() || !usb_serial_get_control());
		
		// Send a message with some instructions.
		sendLine("--- WELCOME TO FALLINGFACES! ---");
		sendLine("Use the w, a, s and d keys to move your player up, left, down and right respectively...");
	}
	
	return level;
}

void setupGame(int level) {
	// Radnom stuff TODO: update this comment.
	lives = 3;
	score = 0;
	srand(randSeed);
	
	// Initialize all our sprites.
    init_sprite(&player, 0, 0, 8, 8, bmPlayer[0]);
	init_sprite(&angry, 0, 0, 8, 8, bmAngry);
	init_sprite(&smiley, 0, 0, 8, 8, bmSmiley);
	init_sprite(&crazy, 0, 0, 8, 8, bmCrazy);
	
	// Set some initial values for the Sprites.
	player.dx	= 0.5;
	player.dy	= 0.2;
	angry.dx	= 0;
	angry.dy	= 0.2;
	smiley.dx	= 0;
	smiley.dy	= 0.2;
	crazy.dx	= 0;
	crazy.dy	= 0.2;
	
	// Set all the sprites to not be visible so they will initially spawn.
	player.is_visible	= 0;
	angry.is_visible	= 0;
	smiley.is_visible	= 0;
	crazy.is_visible	= 0;
	
	// Set the initial spawn location for the Player and Faces.
	spawnPlayer(level);
	spawnFaces(level);
}

void startGame() {
	while (1) {
		clear_screen();			// Clear the Screen.
		
		winLoose();				// Check for a Win or Loose condition.
		
		moveFaces();			// Move the faces that are currently vissible.
		collisionDetection();	// Detect collision with the sides of the screen, player or other faces.
		
		spawnPlayer(level);		// Spawn the Player if required.
		spawnFaces(level);		// Spawn the Faces if required.
		
		// Draw everything to the screen and show the screen.
		draw_sprite(&player);
		draw_sprite(&angry);
		draw_sprite(&smiley);
		draw_sprite(&crazy);
		drawBorder();
		drawBanner(lives, score, level);
		
		show_screen();			// Show the Screen.
		_delay_ms(50);			// Delay for x ms to make things smoother.
	}
}

void drawBorder() {
	draw_line(0, 0, 83, 0);		// Top
	draw_line(83, 0, 83, 47);	// Right
	draw_line(83, 47, 0, 47);	// Bottom
	draw_line(0, 47, 0, 0);		// Left
}

void drawBanner(int lives, int score, int level) {
	// Display the players Lives.
	sprintf(buff, "L:%d", lives);
	draw_string(3, 2, buff);
	
	draw_line(20, 0, 20, 10);
	
	// Display the players Score.
	sprintf(buff, "S:%02d", score);
	draw_string(23, 2, buff);
	
	draw_line(45, 0, 45, 10);
	
	// Display the current Level.
	sprintf(buff, "Lvl:%d", level);
	draw_string(52, 2, buff);
	
	draw_line(0, 10, 83, 10);
}

void spawnPlayer(int level) {
	if (!player.is_visible) {
		// Spawn the player based on level.
		if (level == 1) {
			player.x = LCD_X / 2;			// Set x to be the centre of the screen.
			player.y = LCD_Y - 10;			// Set y to be the bottom of the screen.
		} else if (level == 2) {
			player.x = potPosition();		// Set x to be based on the position of POT1.
			player.y = LCD_Y - 10;			// Set y to be the bottom of the screen.
		} else if (level == 3) {
			player.x = (rand() % 72) + 2;	// Set x to be a random number between 2 and 74.
			player.y = (rand() % 26) + 12;	// Set y to be a random number between 12 and 38.
			//TOOD: Direction of travel.
		}
		
		// Ensure the spawn location is 'safe', if not respawn.
		if ((abs(player.x - angry.x) < 13 || abs(player.x - smiley.x) < 13 || abs(player.x - crazy.x) < 13) && (abs(player.y - angry.y) < 13 || abs(player.y - smiley.y) < 13 || abs(player.y - crazy.y) < 13)) {
			spawnPlayer(level);
		} else {
			player.is_visible = 1;
		}
	}
}

void spawnFaces(int level) {
	// Generate new safe spawn location for any face that's not currently visible.
	if (!angry.is_visible) {
		angry.x = (rand() % 72) + 2;
		angry.y = (level == 3) ? (rand() % 26) + 12 : 12;
		
		// Ensure the Angry Face isn't too close to another Face or the Player to spawn.
		if (((abs(angry.x - smiley.x)) < 13 && (abs(angry.y - smiley.y)) < 13) || ((abs(angry.x - crazy.x)) < 13 && (abs(angry.y - crazy.y)) < 13) || ((abs(angry.x - player.x)) < 13 && (abs(angry.y - player.y)) < 13)) {
			spawnFaces(level);
		} else {
			angry.is_visible = 1;
		}
		
		// For level 3 - Give the face a random dx/dy value.
		if (level == 3) {
			angry.dx = sin(rand() % 360);
			angry.dy = cos(rand() % 360);
			
			float distance = sqrt(pow(angry.dx, 2) + pow(angry.dy, 2));
			
			angry.dx /= distance;
			angry.dy /= distance;
		}
	}
	
	if (!smiley.is_visible) {
		smiley.x = (rand() % 72) + 2;
		smiley.y = (level == 3) ? (rand() % 26) + 12 : 12;
		
		if (((abs(smiley.x - angry.x)) < 13 && (abs(smiley.y - angry.y)) < 13) || ((abs(smiley.x - crazy.x)) < 13 && (abs(smiley.y - crazy.y)) < 13) || ((abs(smiley.x - player.x)) < 13 && (abs(smiley.y - player.y)) < 13)) {
			spawnFaces(level);
		} else {
			smiley.is_visible = 1;
		}
		
		// For level 3 - Give the face a random dx/dy value.
		if (level == 3) {
			smiley.dx = sin(rand() % 360);
			smiley.dy = cos(rand() % 360);
			
			float distance = sqrt(pow(smiley.dx, 2) + pow(smiley.dy, 2));
			
			smiley.dx /= distance;
			smiley.dy /= distance;
		}
	}
	
	if (!crazy.is_visible) {
		crazy.x = (rand() % 72) + 2;
		crazy.y = (level == 3) ? (rand() % 26) + 12 : 12;
		
		if (((abs(crazy.x - angry.x)) < 13 && (abs(crazy.y - angry.y)) < 13) || ((abs(crazy.x - smiley.x)) < 13 && (abs(crazy.y - smiley.y)) < 13) || ((abs(crazy.x - player.x)) < 13 && (abs(crazy.y - player.y)) < 13)) {
			spawnFaces(level);
		} else {
			crazy.is_visible = 1;
		}
		
		// For level 3 - Give the face a random dx/dy value.
		if (level == 3) {
			crazy.dx = sin(rand() % 360);
			crazy.dy = cos(rand() % 360);
			
			float distance = sqrt(pow(crazy.dx, 2) + pow(crazy.dy, 2));
			
			crazy.dx /= distance;
			crazy.dy /= distance;
		}
	}
}

void moveFaces() {
	// Move all of the faces according to their dx/dy values.
	angry.x += angry.dx * speed[speedMod];
	angry.y += angry.dy * speed[speedMod];
	smiley.x += smiley.dx * speed[speedMod];
	smiley.y += smiley.dy * speed[speedMod];
	crazy.x += crazy.dx * speed[speedMod];
	crazy.y += crazy.dy * speed[speedMod];
}

void collisionDetection() {
	if (level == 1 || level == 2) {
		// Respawn faces if they leave the screen.
		angry.is_visible = (angry.y >= LCD_Y - 1) ? 0 : 1;
		smiley.is_visible = (smiley.y >= LCD_Y - 1) ? 0 : 1;
		crazy.is_visible = (crazy.y >= LCD_Y - 1) ? 0 : 1;
	} else if (level == 3) {
		// Face to Border Collision.
		(angry.x <= 2 || angry.x >= LCD_X - 9) ? angry.dx *= -1 : 0;
		(angry.y <= 12 || angry.y >= LCD_Y - 9) ? angry.dy *= -1 : 0;
		
		(smiley.x <= 2 || smiley.x >= LCD_X - 9) ? smiley.dx *= -1 : 0;
		(smiley.y <= 12 || smiley.y >= LCD_Y - 9) ? smiley.dy *= -1 : 0;
		
		(crazy.x <= 2 || crazy.x >= LCD_X - 9) ? crazy.dx *= -1 : 0;
		(crazy.y <= 12 || crazy.y >= LCD_Y - 9) ? crazy.dy *= -1 : 0;
		
		// Check for Face to Face Collision.
		int collisionAngrySmiley = ((angry.x < (smiley.x + 7) && ((angry.x + 7) > smiley.x) && (angry.y < (smiley.y + 7)) && ((angry.y + 7) > smiley.y)));	// Angry and Smiley
		int collisionAngryCrazy = ((angry.x < (crazy.x + 7) && ((angry.x + 7) > crazy.x) && (angry.y < (crazy.y + 7)) && ((angry.y + 7) > crazy.y)));		// Angry and Crazy
		int collisionSmileyCrazy = ((smiley.x < (crazy.x + 7) && ((smiley.x + 7) > crazy.x) && (smiley.y < (crazy.y + 7)) && ((smiley.y + 7) > crazy.y)));	// Smiley and Crazy
		
		// Evaluate Face to Face Collision to be Horisontal or Vertical.
		if (collisionAngrySmiley) {
			(debug) ? sendLine("Collision between Angry and Smiley") : 0;
			
			// Identify Vertical or Horisontal Collision.
			int vertical = !(((angry.x + 4) > (smiley.y + 3)) || ((angry.y + 3) < (smiley.y + 4)));
			int horisontal = !(((angry.x + 3) < (smiley.x + 4)) || ((angry.x + 4) > (smiley.x + 3)));
			
			// Invert the relivant vector based on the collision direction.
			if (vertical) { angry.dy *= -1; smiley.dy *= -1; }
			else if (horisontal) { angry.dx *= -1; smiley.dx *= -1; }
		}
		
		if (collisionAngryCrazy) {
			(debug) ? sendLine("Collision between Angry and Crazy") : 0;
			
			// Identify Vertical or Horisontal Collision.
			int vertical = !(((angry.x + 4) > (crazy.y + 3)) || ((angry.y + 3) < (crazy.y + 4)));
			int horisontal = !(((angry.x + 3) < (crazy.x + 4)) || ((angry.x + 4) > (crazy.x + 3)));
			
			// Invert the relivant vector based on the collision direction.
			if (vertical) { angry.dy *= -1; crazy.dy *= -1; }
			else if (horisontal) { angry.dx *= -1; crazy.dx *= -1; }
		}
		
		if (collisionSmileyCrazy) {
			(debug) ? sendLine("Collision between Smiley and Crazy") : 0;
			
			// Identify Vertical or Horisontal Collision.
			int vertical = !(((smiley.x + 4) > (crazy.y + 3)) || ((smiley.y + 3) < (crazy.y + 4)));
			int horisontal = !(((smiley.x + 3) < (crazy.x + 4)) || ((smiley.x + 4) > (crazy.x + 3)));
			
			// Invert the relivant vector based on the collision direction.
			if (vertical) { smiley.dy *= -1; crazy.dy *= -1; }
			else if (horisontal) { smiley.dx *= -1; crazy.dx *= -1; }
		}
	}
	
	// Player collision with each face.
	//if ((abs(player.x - angry.x)) < 8 && (abs(player.y - angry.y) < 8)) {
	//	lives -= 1;
	//	angry.is_visible = 0;
	//}
	//if ((abs(player.x - smiley.x)) < 8 && (abs(player.y - smiley.y) < 8)) {
	//	score += 2;
	//	smiley.is_visible = 0;
	//}
	//if ((abs(player.x - crazy.x)) < 8 && (abs(player.y - crazy.y) < 8)) {
	//	(speedMod == 2) ? speedMod = 0 : speedMod++;
	//	crazy.is_visible = 0;
	//}
}

void winLoose() {
	if (lives <= 0) {
		drawCentred(15, "Oh No!");
		drawCentred(25, "The Game!");
		show_screen();
		(debug) ? _delay_ms(250) : _delay_ms(3000);
		clear_screen();
		fallingFaces();
	} // Loose Condition.
	else if (score >= 20) {
		drawCentred(15, "Winner Winner!");
		drawCentred(30, "Chicken Dinner!");
		show_screen();
		(debug) ? _delay_ms(250) : _delay_ms(3000);
		clear_screen();
		fallingFaces();
	} // Win Condition.
}

uint16_t adc_read(uint8_t ch) {
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'ch' between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing

	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);

	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));

	return (ADC);
}

//--------------------------------------------------
// Helper Functions
//--------------------------------------------------
void drawCentred(unsigned char y, char* string) {
	unsigned char l = 0, i = 0;
	while (string[i] != '\0') { l++; i++; }
	char x = 42-(l*5/2);
	draw_string((x > 0) ? x : 0, y, string);
}

void debugPrint() {
	clear_screen();
	drawCentred(10, "It fookin worked!");
	show_screen();
}

int potPosition() {
	uint16_t adc = adc_read(1);
	
	float max_adc = 1023.0;
	long max_lcd_adc = (adc*(long)(LCD_X - 12)) / max_adc;
	
	return max_lcd_adc + 2;
}

void sendLine(char* string) {
    // Send all of the characters in the string
    unsigned char char_count = 0;
    while (*string != '\0') {
        usb_serial_putchar(*string);
        string++;
        char_count++;
    }

    // Go to a new line (force this to be the start of the line)
    usb_serial_putchar('\r');
    usb_serial_putchar('\n');
}

//--------------------------------------------------
// Interrupt Service Routines (ISR)
//--------------------------------------------------
ISR(TIMER0_OVF_vect) {
	// Handle input and player movement for each level.
	if (level == 1) {
		// Detect SW3 - Incrament the players x location by the value of dx.
		((PINF>>PF5) & 1) ? (player.x < 74) ? player.x += player.dx : 0 : 0;
		// Detect SW2 - Decrement the players x location by the value of dx.
		((PINF>>PF6) & 1) ? (player.x > 2) ? player.x -= player.dx : 0 : 0;
	} else if (level == 2) {
		// Update the players x location based on the value of POT1 relitive to the LCD.
		player.x = potPosition();
	} else if (level == 3) {
		int16_t input = usb_serial_getchar();
		
		player.y += (input == 'w') ? -player.dy : (input == 's') ? player.dy : 0;
		player.x += (input == 'a') ? -player.dx : (input == 'd') ? player.dx : 0;
	}
	
	// Switch between the two player bitmaps.
	playerSprite++;
	if (playerSprite > 20) {
		player.bitmap = (player.bitmap == bmPlayer[0]) ? player.bitmap = bmPlayer[1] : (player.bitmap == bmPlayer[1]) ? player.bitmap = bmPlayer[0] : 0;
		playerSprite = 0;
	}
	
	// Debugging stuff to help with testing
	if ((PIND>>PD1) & 1) {
		angry.is_visible	= 0 ;
		smiley.is_visible	= 0 ;
		crazy.is_visible	= 0 ;
		spawnFaces(level);
	} // Up - Respawns all faces.
	if ((PINB>>PB1) & 1) {
		(speedMod == 2) ? speedMod = 0 : speedMod++;
	} // Left - Increase the game speed.
	if ((PINB>>PB7) & 1) {
		score += 2;
	} // Down - Increase the score.
	if ((PIND>>PD0) & 1) {
		lives -= 1;
	} // Right - Remove a life.
}