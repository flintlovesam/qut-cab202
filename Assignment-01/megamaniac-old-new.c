//------------------------------------------------------------------//
//    CAB202 Assessment 1: Megamaniac - Tylor Stewart (N9013555)    //
//------------------------------------------------------------------//

// Include required standard libraries.
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

// ZDK04 Includes - To be removed later.
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"

// Data Structure for Game Data
typedef struct Game {
    // Game Settings
    int game_level;                     // Current level of game.
    bool game_over;                     // Determines if the game is over (When a player runs out of lives).

    // Player - Spaceship
    int player_lives;                   // Players Lives.
    int player_score;                   // Players Score.
    sprite_id player_spaceship;         // Sprite - Spaceship.
    sprite_id player_missile;           // Sprite - Missile.
	bool missileZ;                      // Is Missile Z currently charging?
    bool missileC;                      // Is Missile C currently charging?
	int missileCharge;                  // Value of charge for Missile, increases curvature.
	bool missileReleasedZ;
    bool missileReleasedC;
	int notZ;

    // Enemy - Aliens
    int enemy_start[10][2];             // Array - Start Locations of Aliens.
    sprite_id enemy_aliens[10];         // Sprite Array - Aliens.
    sprite_id enemy_bombs[4];           // Sprite Array - Bombs.
	int enemy_lastbomb;                 // Sprite ID of the last Alien to drop a bomb.
	timer_id enemy_bombtimer;           // Timer for dropping bombs.
    int enemy_alive;                    // Number of enemies alive.
} Game;

// This will setup the game when it's launched. Stuff like score and lives etc.
void SetupGame(Game * game) {
    // General Settings
    game->game_level = 0;
    game->game_over = false;

    // Player - Spaceship
    game->player_lives = 3;
	game->player_score = 0;

    int startX = screen_width() / 2;
    int startY = screen_height() - 4;
    game->player_spaceship = create_sprite(startX, startY, 3, 1, "|^|");
    game->player_spaceship->dx = 1;

    game->player_missile = create_sprite(0, 0, 1, 1, "|");
    game->player_missile->dy = 1;
    game->player_missile->is_visible = false;

    // Enemy - Aliens
	int start[10][2] = {{8, 2}, {14, 2}, {20, 2}, {5, 4}, {11, 4}, {17, 4}, {23, 4}, {8, 6}, {14, 6}, {20, 6}};

	for (int i = 0; i < 10; i++) {
		game->enemy_start[i][0] = start[i][0];
		game->enemy_start[i][1] = start[i][1];
	}

	for (int i = 0; i < 10; i++) { game->enemy_aliens[i] = create_sprite(game->enemy_start[i][0], game->enemy_start[i][1], 1, 1, "@"); }
	game->enemy_alive = 10;

	game->enemy_bombtimer = create_timer(3000);

	for (int i = 0; i < 4; i++) {
		game->enemy_bombs[i] = create_sprite(0, 0, 1, 1, "Y");
		game->enemy_bombs[i]->dy = 1;
		game->enemy_bombs[i]->is_visible = false;
	}

	game->enemy_lastbomb = 10;

	// Random Setup
	srand(time(NULL));

	game->missileReleasedZ = false;
	game->missileReleasedC = false;
	game->notZ = 0;
}

void ChangeLevel(Game * game) {
	// Change Level.
	(game->game_level == 5) ? game->game_level = 1 : game->game_level++;

	// Check what level needs to be set and configure necessary settings.
	if (game->game_level == 1) {
		for (int i = 0; i < 10; i++) {
			game->enemy_aliens[i]->dx = 0.1;
		}
	} else if (game->game_level == 2) {
		for (int i = 0; i < 10; i++) {
			game->enemy_aliens[i]->dx = 0.1;
		}
	} else if (game->game_level == 3) {
		for (int i = 0; i < 10; i++) {
			game->enemy_aliens[i]->dx = 0.1;
			game->enemy_aliens[i]->dy = 0.1;
		}
	} else if (game->game_level == 4) {
		// Settings code for Level 4
	} else if (game->game_level == 5) {
		// Settings code for Level 5
	}

	// Reset Aliens Location.
	for (int i = 0; i < 10; i++) {
		game->enemy_aliens[i]->x = game->enemy_start[i][0];
		game->enemy_aliens[i]->y = game->enemy_start[i][1];
	}
}

void GameStats(Game * game) {
	// Draw my Name and Student Number.
	draw_line(0, screen_height() - 3, screen_width() - 1, screen_height() - 3, '_');
	draw_string(1, screen_height() - 2, "Tylor Stewart (N9013555)");

	// Draw the current Score of the Player.
	draw_string(screen_width() - 23, screen_height() - 2, "Score = ");
	draw_int(screen_width() - 15, screen_height() - 2, game->player_score);

	// Draw the current Lives of the Player.
	draw_string(screen_width() - 10, screen_height() - 2, "Lives = ");
	draw_int(screen_width() - 2, screen_height() - 2, game->player_lives);

	// Draw the current Level of the Game the Player is on.
	int levelX = screen_width() / 2;
	int levelY = screen_height() - 1;
	if (game->game_level == 1) {
		char levelName[] = "Level 1: Basic";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->game_level == 2) {
		char levelName[] = "Level 2: Harmonic";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->game_level == 3) {
		char levelName[] = "Level 3: Falling";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->game_level == 4) {
		char levelName[] = "Level 4: Drunken";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	} else if (game->game_level == 5) {
		char levelName[] = "Level 5: Aggressive";
		levelX -= strlen(levelName) / 2;
		draw_string(levelX, levelY, levelName);
	}
}

int UserInput(Game * game) {
	int key = get_char();
	if (key == 'l') {
		ChangeLevel(game);
	} else if (key == 'r') {
		// Restart Game.
	} else if (key == 'p') {
		// Pause Game.
	} else if (key == 'q') {
		game->game_over = true;
	}
	return key;
}

void PlayerMovement(Game * game, int userInput) {
	// Player - Spaceship & Player - Missile: User Input.
	if (userInput == 'a' && game->player_spaceship->x >= 1) {
		game->player_spaceship->x -= game->player_spaceship->dx;
	} else if (userInput == 'd' && game->player_spaceship->x <= screen_width() - 4) {
		game->player_spaceship->x += game->player_spaceship->dx;
	} else if (userInput == 's' && !game->player_missile->is_visible) {
		game->player_missile->x = game->player_spaceship->x + 1;
		game->player_missile->y = game->player_spaceship->y;
		game->player_missile->dx = 0;
		game->player_missile->dy = 1;
		game->player_missile->is_visible = true;
	} else if ((userInput == 'z' && game->missileCharge < 100) || (userInput == -1 && game->missileCharge == 1)) {
		game->missileCharge += 1;
		game->notZ = 0;
	} else if (userInput != 'z' && game->missileCharge > 2) {
		if (game->notZ > 1) {
			game->missileReleasedZ = true;
			game->missileCharge = 0;
			game->notZ = 0;

			game->player_missile->x = game->player_spaceship->x;
			game->player_missile->y = game->player_spaceship->y;
			game->player_missile->dx = 0;
			game->player_missile->dy = 0.5;
			game->player_missile->is_visible = true;
		} else {
			game->notZ++;
		}
	} else if (userInput == 'c') {
		game->player_missile->x = game->player_spaceship->x + 3;
		game->player_missile->y = game->player_spaceship->y;
		game->player_missile->dx = 0;
		game->player_missile->dy = 0.5;
		game->player_missile->is_visible = true;
	}

	// Player - Spaceship: Location is dynamic based on screen dimensions.
	game->player_spaceship->y = screen_height() - 4;

	//Player - Missile: Curve.
	if (game->missileZ) {
		game->player_missile->dx += 0.01;
	}

	// Player - Missile: Movement.
	if (game->player_missile->is_visible) {
		game->player_missile->x -= game->player_missile->dx;
		game->player_missile->y -= game->player_missile->dy;
		if (game->player_missile->y < 0) {
			game->player_missile->is_visible = false;
		}
	}
}

int SelectAlien(Game * game) {
	// Randomly select an Alien that is alive.
	int alien = rand() % 10;
	if (game->enemy_aliens[alien]->is_visible && alien != game->enemy_lastbomb) {
		return alien;
	} else if (game->enemy_aliens[alien]->is_visible && game->enemy_alive == 1) {
		return alien;
	} else {
		return SelectAlien(game);
	}
}

void EnemyMovement(Game * game) {
	// Enemy - Aliens: Movement based on Level.
	for (int i = 0; i < 10; i++) {
		if (game->game_level == 1) {
			game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
		} else if (game->game_level == 2) {
			//
			double y = 2 * sin((game->enemy_aliens[3]->x * 2 * M_PI) / (screen_width() / 6));

			// Offset the top and bottom row of aliens back to their correct formation.
			(i == 0 || i == 1 || i == 2) ? y -= 2 : (i == 7 || i == 8 || i == 9) ? y += 2 : y;

			//
			game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
			game->enemy_aliens[i]->y = y + 5;
		} else if (game->game_level == 3) {
			game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
			game->enemy_aliens[i]->y += game->enemy_aliens[i]->dy;
		} else if (game->game_level == 4) {
			// Movement code for Level 4
		} else if (game->game_level == 5) {
			// Movement code for Level 5
		}

		// Wrapping - Horizontal
		if (game->enemy_aliens[i]->x > screen_width() - 1) {
			game->enemy_aliens[i]->x = 0;
		}

		// Wrapping - Vertical
		if (game->enemy_aliens[i]->y > screen_height() - 4) {
			game->enemy_aliens[i]->y = 0;
		}
	}

	// Enemy - Bombs: Launching
	if (timer_expired(game->enemy_bombtimer)) {
		int alien = SelectAlien(game);
		for (int i = 0; i < 4; i++) {
			if (!game->enemy_bombs[i]->is_visible) {
				game->enemy_bombs[i]->x = game->enemy_aliens[alien]->x;
				game->enemy_bombs[i]->y = game->enemy_aliens[alien]->y;
				game->enemy_bombs[i]->is_visible = true;
				game->enemy_lastbomb = alien;
				break;
			}
		}
	}

	// Enemy - Bombs: Movement
	for (int i = 0; i < 4; i++) {
		if (game->enemy_bombs[i]->is_visible) { game->enemy_bombs[i]->y += game->enemy_bombs[i]->dy; }
	}
}

void RespawnPlayer(Game * game) {
	game->player_spaceship->x = screen_width() / 2;
	game->player_spaceship->y = screen_height() - 4;
	game->player_spaceship->is_visible = true;
}

void CollisionEvents(Game * game) {
	// Enemy - Aliens: Re-Spawn Aliens
	if(game->enemy_alive == 0) {
		for (int i = 0; i < 10; i++) {
			game->enemy_aliens[i]->is_visible = true;
		}
		game->enemy_alive = 10;
	}

	// Player - Missile: Collision with Enemy - Aliens
	for (int i = 0; i < 10; i++) {
		if (round(game->enemy_aliens[i]->x) == round(game->player_missile->x) && round(game->enemy_aliens[i]->y) == round(game->player_missile->y) && game->enemy_aliens[i]->is_visible && game->player_missile->is_visible) {
			game->enemy_aliens[i]->is_visible = false;
			game->player_missile->is_visible  = false;
			game->enemy_alive -= 1;
		}
	}

	// Enemy - Bomb Collision with...
	for (int i = 0; i < 4; i++) {
		//  Bottom of Screen
		if (game->enemy_bombs[i]->is_visible && game->enemy_bombs[i]->y > screen_height() - 4) {
			game->enemy_bombs[i]->is_visible = false;
		}

		// Player - Spaceship
		if (game->enemy_bombs[i]->is_visible && round(game->enemy_bombs[i]->x) >= game->player_spaceship->x && round(game->enemy_bombs[i]->x) <= (game->player_spaceship->x + game->player_spaceship->width - 1) && round(game->enemy_bombs[i]->y) == game->player_spaceship->y) {
			game->enemy_bombs[i]->is_visible = false;
			RespawnPlayer(game);
			game->player_lives -= 1;
		}
	}
}

void UpdateScreen(Game * game) {
	// Draw the Player - Spaceship
	draw_sprite(game->player_spaceship);

	// Draw the Player - Missile
	draw_sprite(game->player_missile);

	// Draw the Enemy - Aliens
	for (int i = 0; i < 10; i++) { draw_sprite(game->enemy_aliens[i]); };

	//  Draw the Enemy - Bombs
	for (int i = 0; i < 4; i++) { draw_sprite(game->enemy_bombs[i]); };

	// Debugging Draws...
	draw_double(0, 1, game->missileZ);
	draw_double(0, 2, game->missileC);
	draw_double(0, 3, game->missileCharge);
	draw_double(0, 4, game->missileReleasedZ);
	draw_double(0, 5, game->notZ);
}

// This is my primary Event Loop. This is where most of the fun stuff happens ;)
void megamaniac() {
    // WTF does this do? Something to do with my Data Structure I assume...
    Game game;

    // Let's run our initial setup to get everything for the game ready.
    SetupGame(&game);

    // Do we need to change the level? Not really but we should probably start at Level 1.
    ChangeLevel(&game);

    // This is where the Event Loop actually starts. This will run repeatedly until our game is over.
    while (!game.game_over) {
	    clear_screen();
	    GameStats(&game);

	    int userInput = UserInput(&game);
	    draw_double(0, 0, userInput);

	    PlayerMovement(&game, userInput);
	    EnemyMovement(&game);
	    CollisionEvents(&game);

		UpdateScreen(&game);
	    show_screen();
	    timer_pause(25);
    }
}

// This is Main, there are many others like it but this is the main one...
int main() {
    setup_screen();
    megamaniac();
    cleanup_screen();
    return 0;
}
