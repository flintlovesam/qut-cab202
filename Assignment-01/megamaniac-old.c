// CAB202 Assessment 1: Megamaniac - Tylor Stewart (N9013555)

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ZDK04 Includes - To be removed later.
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"

// Data Structure to hold Game Data.
typedef struct Game {
	timer_id gameTimer;
	timer_id bombTimer;
	sprite_id spaceship;
	sprite_id missile;
	sprite_id aliens[10];
	sprite_id bombs[4];
	int score;
	int lives;
	int level;
	bool over;
	int aliensAlive;
	int bombLast;
} Game;

// Game Setup
void setup(Game * game) {
	// General Settings
	game->lives = 3;
	game->score = 0;
	game->level = 1;
	game->over = false;

	// Player - Spaceship
	int startX = screen_width() / 2;
	int startY = screen_height() - 4;
	game->spaceship = create_sprite(startX, startY, 3, 1, "|^|");
	game->spaceship->dx = 1;

	// Player - Missile
	game->missile = create_sprite(0, 0, 1, 1, "|");
	game->missile->is_visible = false;
	game->missile->dy = 2;

	// Aliens - Starting Locations
	int aliensStart[10][2] = {{8, 2}, {14, 2}, {20, 2}, {5, 4}, {11, 4}, {17, 4}, {23, 4}, {8, 6}, {14, 6}, {20, 6}};

	// Aliens - Create
	for (int i = 0; i < 10; i++) {
		game->aliens[i] = create_sprite(aliensStart[i][0], aliensStart[i][1], 1, 1, "@");
	}
	game->aliensAlive = 10;

	// Bombs - Create
	for (int i = 0; i < 4; i++) {
		game->bombs[i] = create_sprite(0, 0, 1, 1, "Y");
		//game->bombs[i]->is_visible = false;
	}

	// Configuration
	game->gameTimer = create_timer(60);
	game->bombTimer = create_timer(3000);
}

void changeLevel(Game * game, int level) {
	if (level == 1) {
		for (int i = 0; i < 10; i++) {
			game->aliens[i]->dx = 0.1;
			game->aliens[i]->dy = 0;
		}
	} else if (level == 2) {}
		for (int i = 0; i < 10; i++) {
			game->aliens[i]->dx = 0.1;
			game->aliens[i]->dy = 0;
		}
}

// Draw the User Interface
void userInterface(Game * game) {
	// Draw my Name and Student Number
	draw_line(0, screen_height() - 3, screen_width() - 1, screen_height() - 3, '_');
	draw_string(1, screen_height() - 2, "Tylor Stewart (N9013555)");

	// Draw the current Score of the Player
	draw_string(screen_width() - 23, screen_height() - 2, "Score = ");
	draw_int(screen_width() - 15, screen_height() - 2, game->score);

	// Draw the current Lives of the Player
	draw_string(screen_width() - 10, screen_height() - 2, "Lives = ");
	draw_int(screen_width() - 2, screen_height() - 2, game->lives);

	// Draw the current Level the Player is on
	int levelX = screen_width() / 2;
	int levelY = screen_height() - 1;
	if (game->level == 1) {
		char levelName[] = "Level 1: Basic";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->level == 2) {
		char levelName[] = "Level 2: Harmonic";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->level == 3) {
		char levelName[] = "Level 3: Falling";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->level == 4) {
		char levelName[] = "Level 4: Drunken";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->level == 5) {
		char levelName[] = "Level 5: Aggressive";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	}
}

void playerInput(Game * game) {
	// Get key input and perform the relevant action.
	int key = get_char();
	if (key == 'a' && game->spaceship->x >= 0) {
		// Move the players spacecraft left.
		game->spaceship->x -= game->spaceship->dx;
	} else if (key == 'd' && game->spaceship->x <= screen_width() - 5) {
		// Move the players spacecraft right.
		game->spaceship->x += game->spaceship->dx;
	} else if (key == 's') {
		// Shoot shit at enemies.
		if (!game->missile->is_visible) {
			game->missile->is_visible = true;
			game->missile->x = game->spaceship->x + 1;
			game->missile->y = game->spaceship->y - 1;
		}
	} else if (key == 'r') {
		// Restart (confirm first?).
	} else if (key == 'q') {
		game->over = true;
	}
}

void updateScreen(Game * game) {
	// Automatic Movement - Missile
	if (game->missile->is_visible) {
		game->missile->y -= game->missile->dy;
		if (game->missile->y < 0) {
			game->missile->is_visible = false;
		}
	}

	// Automatic Movement - Aliens
	for (int i = 0; i < 10; i++) {
		game->aliens[i]->x += game->aliens[i]->dx;
		game->aliens[i]->y += game->aliens[i]->dy;

		// Screen Looping
		if(game->aliens[i]->x > screen_width() - 1) {
			game->aliens[i]->x = 0;
		}

		// Missile Collision with Alien
		if (round(game->aliens[i]->x) == round(game->missile->x) && round(game->aliens[i]->y) == round(game->missile->y) && game->aliens[i]->is_visible) {
			game->aliens[i]->is_visible = false;
			game->aliensAlive -= 1;
		}

		// Draw the Aliens
		draw_sprite(game->aliens[i]);
	}

	// Automatic Movement - Bombs
	for (int i = 0; i < 4; i++) {
		// Calculate New Position
		game->bombs[i]->x += game->bombs[i]->dx;
		game->bombs[i]->y += game->bombs[i]->dy;

		// Hitting bottom of Screen

		// Collision with Player

		// Draw the Bomb
		draw_sprite(game->bombs[i]);
	}

	// Bombs - Drop New
	if(timer_expired(game->bombTimer)) {
		// Generate a random number between 0 and 9 to used as the aliens ID.
		srand(time(NULL));
		int r = rand() % 9;

		// Itterate through the Bomb Sprites until we find one that's not in use.
		for (int i = 0; i < 4; i++) {
			if (!game->bombs[i]->is_visible) {
				game->bombs[i]->x = game->aliens[r]->x;
				game->bombs[i]->y = game->aliens[r]->y + 1;
				game->bombs[i]->is_visible = true;
				draw_sprite(game->bombs[i]);
				break;
			}
		}
	}

	// Respawn - Aliens
	if(game->aliensAlive == 0) {
		for (int i = 0; i < 10; i++) {
			game->aliens[i]->is_visible = true;
		}
		game->aliensAlive = 10;
	}

	// Respawn - Player

	// Draw  remaining Sprites
	draw_sprite(game->spaceship);
	draw_sprite(game->missile);

}

void megamaniac() {
	Game game; // WTF does this do?

	setup(&game);
	changeLevel(&game, 1);
	while (!game.over) {
		clear_screen();
		userInterface(&game);
		playerInput(&game);
		updateScreen(&game);
		show_screen();
		timer_pause(30);
	}
}

int main() {
	setup_screen();
	megamaniac();
	wait_char();
	cleanup_screen();
	return 0;
}