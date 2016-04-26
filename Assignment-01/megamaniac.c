//------------------------------------------------------------------//
//		CAB202 Assessment 1: Megamaniac - Tylor Stewart (n9013555)	//
//------------------------------------------------------------------//
// Bonus Content:													//
// 1. 50% chance to drop one of two powerups when an alien dies.	//
// 2. Missile Powerup - Gives the player an extra 10 missiles		//
//		that they can shoot at will.								//
// 3. Sheild Upgrade - Gives the player a sheild, protecting them	//
//		from a single impact.										//
//------------------------------------------------------------------//

// Include required standard libraries.
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

// ZDK04 Includes - To be removed later if time permits.
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"

typedef struct Game {
    // General Game Settings
    int game_level;                     // Current level of game.
    bool game_over;                     // Determines if the game is over (When a player runs out of lives).
	timer_id levelFiveTimer;

    // Player - Spaceship
    int player_lives;                   // Players Lives.
    int player_score;                   // Players Score.
    sprite_id player_spaceship;         // Sprite - Spaceship.
    sprite_id player_missile[5];       	// Sprite Array - Missile.
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

	// Bonus Content
	sprite_id bonus_sheild;
	sprite_id bonus_missiles;
	sprite_id player_sheild;

	// Level Four Stuff
	double LevelFour_borderLeft;
	double LevelFour_borderRight;
	bool levelFour_contract;

	// Level Five Stuff
	int levelFive_stage;
	timer_id levelFive_timer;
	int levelFive_selected;

	double levelFive_displacement;

	int levelFive_playerStartX;
	int levelFive_playerStartY;

	int levelFive_alienStartX;
	int levelFive_alienStartY;

	// Debugging Stuff
	bool bonusMissilesActive;
	int bonusMissiles;
	int bonusChance;
	int bonusDrop;
} Game;

void Megamaniac();

void SetupGame(Game * game) {
    /*-----| General Game Settings |-----*/
    game->game_level = 0;
    game->game_over = false;
	srand(time(NULL));
	game->levelFive_stage = 0;

    /*-----| Player: Spaceship |-----*/
	// General Settings
    game->player_lives = 3;
	game->player_score = 0;

	// Generate the Sprite for the Spaceship.
    int startX = screen_width() / 2;
    int startY = screen_height() - 4;
    game->player_spaceship = create_sprite(startX, startY, 3, 1, "|^|");
    game->player_spaceship->dx = 1;

	/*-----| Player: Missile |-----*/
	// Generate the Sprite for the Missile.
	for (int i = 0; i < 5; i++) {
		game->player_missile[i] = create_sprite(0, 0, 1, 1, "|");
	    game->player_missile[i]->dy = 1;
	    game->player_missile[i]->is_visible = false;
	}

    /*-----| Enemy: Aliens |-----*/
	// Create an Array of the starting locations for the Aliens.
	int start[10][2] = {{8, 2}, {14, 2}, {20, 2}, {5, 4}, {11, 4}, {17, 4}, {23, 4}, {8, 6}, {14, 6}, {20, 6}};

	// Load the start array into our Data Structure so it can be used later.
	for (int i = 0; i < 10; i++) {
		game->enemy_start[i][0] = start[i][0];
		game->enemy_start[i][1] = start[i][1];
	}

	// Generate the Sprites for the Aliens.
	for (int i = 0; i < 10; i++) {
		game->enemy_aliens[i] = create_sprite(game->enemy_start[i][0], game->enemy_start[i][1], 1, 1, "@");
	}
	game->enemy_alive = 10;

	/*-----| Enemy: Bombs |-----*/
	// General Settings
	game->enemy_bombtimer = create_timer(3000);
	game->enemy_lastbomb = 10;

	for (int i = 0; i < 4; i++) {
		game->enemy_bombs[i] = create_sprite(0, 0, 1, 1, "Y");
		game->enemy_bombs[i]->dy = 1;
		game->enemy_bombs[i]->is_visible = false;
	}

	/*-----| Bonus Content |-----*/
	// General Settings.
	game->bonusChance = -1;
	game->bonusDrop = -1;

	// Sheild Bonus.
	game->bonus_sheild = create_sprite(0, 0, 1, 1, "S");
	game->bonus_sheild->is_visible = false;
	game->player_sheild = create_sprite(0, 0, 3, 1, "___");
	game->player_sheild->is_visible = false;

	// Missiles Bonus
	game->bonus_missiles = create_sprite(0, 0, 1, 1, "M");
	game->bonus_missiles->is_visible = false;
	game->bonusMissiles = 0;

	/*-----| Miscellaneous or Debugging |-----*/
	game->missileReleasedZ = false;
	game->missileReleasedC = false;
	game->notZ = 0;
	game->bonusMissilesActive = false;
	game->levelFive_selected = 0;

	// Left Four Stuff
	game->LevelFour_borderLeft = 4;
	game->LevelFour_borderRight = screen_width() - 5;
	game->levelFour_contract = true;
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

void RespawnPlayer(Game * game) {
	if (game->player_lives == 0) {
		game->game_over = true;
	} else {
		// Respawn the Player in a safe location.
		game->player_spaceship->x = screen_width() / 2;
		game->player_spaceship->y = screen_height() - 4;
		game->player_spaceship->is_visible = true;

		// Reset Bonuses.
		game->bonusMissiles = 0;
	}

	for (int i = 0; i < 4; i++) {
		game->enemy_bombs[i]->is_visible = false;
	}
}

void RespawnAliens(Game * game) {
	for (int i = 0; i < 10; i++) {
		game->enemy_aliens[i]->x = game->enemy_start[i][0];
		game->enemy_aliens[i]->y = game->enemy_start[i][1];
		game->enemy_aliens[i]->is_visible = true;
	}
	game->enemy_alive = 10;
}

void ChangeLevel(Game * game) {
	// Incrament the Game Level, if 5 loop.
	(game->game_level == 5) ? game->game_level = 1 : game->game_level++;

	// Level Specific Settings (if needed).
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
		// Settings for Level 4
	} else if (game->game_level == 5) {
		// Settings for Level 5
	}

	// Reset and Re-Spawn Aliens.
	RespawnAliens(game);

	if (game->game_level != 5) {
		game->levelFive_stage = 0;
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
		Megamaniac();
	} else if (key == 'p') {
		// Pause Game.
	} else if (key == 'q') {
		exit(0);
	}
	return key;
}

void MovementPlayer(Game * game, int userInput) {
	// Move Player based on User Input.
	if (userInput == 'a' && game->player_spaceship->x >= 1) {
		game->player_spaceship->x -= game->player_spaceship->dx;
	} else if (userInput == 'd' && game->player_spaceship->x <= screen_width() - 4) {
		game->player_spaceship->x += game->player_spaceship->dx;
	}

	// Ensure the players Y location is correct.
	game->player_spaceship->y = screen_height() - 4;
}

void MovementMissiles(Game * game, int userInput) {
	// Launching new Missiles.
	if (userInput == 's') {
		if (!game->player_missile[0]->is_visible) {
			game->player_missile[0]->x = game->player_spaceship->x + 1;
			game->player_missile[0]->y = game->player_spaceship->y;
			game->player_missile[0]->dx = 0;
			game->player_missile[0]->dy = 1;
			game->player_missile[0]->is_visible = true;
		} else if (game->bonusMissiles > 0) {
			for (int i = 1; i < 5; i++) {
				if (!game->player_missile[i]->is_visible) {
					game->player_missile[i]->x = game->player_spaceship->x + 1;
					game->player_missile[i]->y = game->player_spaceship->y;
					game->player_missile[i]->dx = 0;
					game->player_missile[i]->dy = 1;
					game->player_missile[i]->is_visible = true;
					game->bonusMissiles--;
					break;
				}
			}
		}
	} else if (userInput == 'z') {

	} else if (userInput == 'c') {

	}

	// Moving existing Missiles.
	for (int i = 0; i < 5; i++) {
		if (game->player_missile[i]->is_visible) {
			// Move the Missile.
			game->player_missile[i]->y -= game->player_missile[i]->dy;
			// Remove the Missile if it's off the screen.
			(game->player_missile[i]->y < 0) ? game->player_missile[i]->is_visible = false : 0;
		}
	}
}

void MovementLevelOne(Game * game) {
	for (int i = 0; i < 10; i++) {
		game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
	}
}

void MovementLevelTwo(Game * game) {
	for (int i = 0; i < 10; i++) {
		// Generate new Y location for Aliens.
		double y = 2 * sin((game->enemy_aliens[3]->x * 2 * M_PI) / (screen_width() / 6));

		// Offset the top and bottom row of aliens back to their correct formation.
		(i == 0 || i == 1 || i == 2) ? y -= 2 : (i == 7 || i == 8 || i == 9) ? y += 2 : y;

		// Move the Aliens
		game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
		game->enemy_aliens[i]->y = y + 5;
	}
}

void MovementLevelThree(Game * game) {
	for (int i = 0; i < 10; i++) {
		game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
		game->enemy_aliens[i]->y += game->enemy_aliens[i]->dy;
	}
}

void MovementLevelFour(Game * game) {
	for (int i = 0; i < 10; i++) {
		double r = (rand() % 100) - 50;
		r -= (i - 5);

		if (game->enemy_aliens[i]->x > game->LevelFour_borderRight) {
			game->enemy_aliens[i]->dx = 0.2;
		}
		if (game->enemy_aliens[i]->x < game->LevelFour_borderLeft) {
			game->enemy_aliens[i]->dx = -0.2;
		}

		// Collision Check - This could get nasty...
		for (int i = 0; i < 10; i++) {
			for (int n = 0; n < 10; n++) {
				if (round(game->enemy_aliens[i]->y) == round(game->enemy_aliens[n]->y) && round(game->enemy_aliens[i]->x) == (round(game->enemy_aliens[n]->x) - 2)) {
					game->enemy_aliens[i]->dx = 0.1;
				}

				if (round(game->enemy_aliens[i]->y) == round(game->enemy_aliens[n]->y) && round(game->enemy_aliens[i]->x) == (round(game->enemy_aliens[n]->x) + 2)) {
					game->enemy_aliens[i]->dx = -0.1;
				}
			}
		}

		//
		game->enemy_aliens[i]->x -= (r / 100) + game->enemy_aliens[i]->dx;
		game->enemy_aliens[i]->dy = 0.1;
		game->enemy_aliens[i]->y += game->enemy_aliens[i]->dy;

		if (game->LevelFour_borderRight > (screen_width() - 5)) {
			game->levelFour_contract = true;
		}
		if (game->LevelFour_borderRight < (game->LevelFour_borderLeft + 17)) {
			game->levelFour_contract = false;
		}

		if (game->levelFour_contract == true) {
			game->LevelFour_borderRight -= 0.01;
			game->LevelFour_borderLeft += 0.01;
		} else {
			game->LevelFour_borderRight += 0.01;
			game->LevelFour_borderLeft -= 0.01;
		}
	}
}

void MovementLevelFive(Game * game) {
	// Generate number between two numbers.
	//int r = (rand() % ((max + 1) - min)) - min;

	if(game->levelFive_stage == 0) {
		//int r = (rand() % ((4000 + 1) - 2000)) - 2000;
		//game->levelFive_timer = create_timer(r);
		game->levelFive_stage = 1;
	} else if (game->levelFive_stage == 1) {
		game->levelFive_selected = SelectAlien(game);
		game->enemy_aliens[game->levelFive_selected]->x--;
		game->levelFive_stage = 2;
	} else if (game->levelFive_stage == 2) {
		if (game->levelFive_selected != 0 && game->enemy_aliens[game->levelFive_selected]->y < game->enemy_aliens[0]->y + 5) {
			game->enemy_aliens[game->levelFive_selected]->y += 0.1;
		} else if (game->levelFive_selected == 0 && game->enemy_aliens[game->levelFive_selected]->y < game->enemy_aliens[1]->y + 5) {
			game->enemy_aliens[game->levelFive_selected]->y += 0.1;
		} else {
			game->levelFive_stage = 3;
		}
	} else if (game->levelFive_stage == 3) {
		if (game->enemy_aliens[game->levelFive_selected]->y < (screen_height() / 2)) {
			game->levelFive_stage = 4;
		}
	} else if (game->levelFive_stage == 4) {
		if (game->enemy_aliens[game->levelFive_selected]->x < (screen_width() / 2)) {
			game->levelFive_displacement = ((rand() % ((-10 + 1) - -30)) - -30) / 100.0;
		} else {
			game->levelFive_displacement = ((rand() % ((30 + 1) - 10)) - 10) / 100.0;
		}

		game->levelFive_playerStartX = game->player_spaceship->x;
		game->levelFive_playerStartY = game->player_spaceship->y;
		game->levelFive_alienStartX = game->enemy_aliens[game->levelFive_selected]->x;
		game->levelFive_alienStartY = game->enemy_aliens[game->levelFive_selected]->y;

		game->levelFive_stage = 5;
	} else if (game->levelFive_stage == 5) {
		double xs = game->levelFive_playerStartX - game->levelFive_alienStartX;
		double ys = game->levelFive_playerStartY - game->levelFive_alienStartY;
		double y = game->enemy_aliens[game->levelFive_selected]->y - game->levelFive_alienStartY;

		double y2 = ys - (xs / (game->levelFive_displacement * ys));
		double x = game->levelFive_displacement * y * (y - y2);

		game->enemy_aliens[game->levelFive_selected]->x = x + game->levelFive_alienStartX;

		if (!game->enemy_aliens[game->levelFive_selected]->is_visible || game->enemy_aliens[game->levelFive_selected]->y > screen_height() - 3.5) {
			game->levelFive_stage = 6;
		}
	} else if (game->levelFive_stage == 6) {
		if (game->levelFive_selected == 0) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[1]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[1]->y;
		} else if (game->levelFive_selected == 1) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[2]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[2]->y;
		} else if (game->levelFive_selected == 2) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[1]->x + 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[1]->y;
		}  else if (game->levelFive_selected == 3) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[4]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[4]->y;
		} else if (game->levelFive_selected == 4) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[5]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[5]->y;
		} else if (game->levelFive_selected == 5) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[6]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[6]->y;
		} else if (game->levelFive_selected == 6) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[5]->x + 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[5]->y;
		} else if (game->levelFive_selected == 7) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[8]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[8]->y;
		} else if (game->levelFive_selected == 8) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[9]->x - 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[9]->y;
		} else if (game->levelFive_selected == 9) {
			game->enemy_aliens[game->levelFive_selected]->x = game->enemy_aliens[8]->x + 6;
			game->enemy_aliens[game->levelFive_selected]->y = game->enemy_aliens[8]->y;
		}

		game->levelFive_selected = -1;
		game->levelFive_stage = 0;
	}

	// Now we can finally move the Aliens!
	for (int i = 0; i < 10; i++) {
		if (i == game->levelFive_selected) {
			game->enemy_aliens[i]->y += game->enemy_aliens[i]->dy;
		} else {
			game->enemy_aliens[i]->x += game->enemy_aliens[i]->dx;
			game->enemy_aliens[i]->y += game->enemy_aliens[i]->dy;
		}
	}

	// Handle wrapping for the selected Alien.
	if (game->levelFive_stage != 5 && game->enemy_aliens[game->levelFive_selected]->y > (screen_height() - 4)) {
		game->enemy_aliens[game->levelFive_selected]->y = 0;
	}
}

void MovementAliens(Game * game) {
	// Move Aliens according to Level
	if (game->game_level == 1) {
		MovementLevelOne(game);
	} else if (game->game_level == 2) {
		MovementLevelTwo(game);
	} else if (game->game_level == 3) {
		MovementLevelThree(game);
	} else if (game->game_level == 4) {
		MovementLevelFour(game);
	} else if (game->game_level == 5) {
		MovementLevelFive(game);
	}

	// Screen Wrapping
	for (int i = 0; i < 10; i++) {
		if (game->enemy_aliens[i]->x > screen_width() - 1) {
			game->enemy_aliens[i]->x = 0;
		}

		if (game->enemy_aliens[i]->y > screen_height() - 4) {
			game->enemy_aliens[i]->y = 0;
		}
	}
}

void MovementBombs(Game * game) {
	// Launch new Bombs.
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

	// Move existing Bombs.
	for (int i = 0; i < 4; i++) {
		if (game->enemy_bombs[i]->is_visible) {
			game->enemy_bombs[i]->y += game->enemy_bombs[i]->dy;
		}
	}
}

void MovementBonuses(Game * game) {
	// Move the Shild Bonus Drop.
	if (game->bonus_sheild->is_visible) {
		game->bonus_sheild->y += game->bonus_sheild->dy;
	}

	// Move the Missile Bonus Drop.
	if (game->bonus_missiles->is_visible) {
		game->bonus_missiles->y += game->bonus_missiles->dy;
	}

	// Move the Player Sheild.
	if (game->player_sheild->is_visible) {
		game->player_sheild->x = game->player_spaceship->x;
		game->player_sheild->y = game->player_spaceship->y - 1;
	}
}

void GenerateBonus(Game * game, int alienID) {
	// Generate some numbers to be used for some things.
	game->bonusChance = rand() % 2;
	game->bonusDrop = rand() % 2;

	// Will we drop a bonus?
	if (game->bonusChance == 0) {
		// Which Bonus will we drop?
		if (game->bonusDrop == 0) {	// Sheild.
			if (!game->bonus_sheild->is_visible) {
				game->bonus_sheild->x = game->enemy_aliens[alienID]->x;
				game->bonus_sheild->y = game->enemy_aliens[alienID]->y + 1;
				game->bonus_sheild->dy = 0.3;
				game->bonus_sheild->is_visible = true;
			}
		} else if (game->bonusDrop == 1) {	// Bullet Speed.
			if (!game->bonus_missiles->is_visible) {
				game->bonus_missiles->x = game->enemy_aliens[alienID]->x;
				game->bonus_missiles->y = game->enemy_aliens[alienID]->y + 1;
				game->bonus_missiles->dy = 0.3;
				game->bonus_missiles->is_visible = true;
			}
		}
	}
}

void CollisionMissiles(Game * game) {
	// Enemy - Aliens: Re-Spawn Aliens
	if(game->enemy_alive == 0) {
		game->player_score += 500;
		RespawnAliens(game);
	}

	// Missile collision with Aliens.
	for (int a = 0; a < 10; a++) {
		for (int m = 0; m < 5; m++) {
			if (round(game->enemy_aliens[a]->x) == round(game->player_missile[m]->x) && round(game->enemy_aliens[a]->y) == round(game->player_missile[m]->y) && game->enemy_aliens[a]->is_visible && game->player_missile[m]->is_visible) {
				game->enemy_aliens[a]->is_visible = false;
				game->player_missile[m]->is_visible  = false;
				game->enemy_alive -= 1;
				game->player_score += 30;
				GenerateBonus(game, a);
			}
		}
	}
}

void CollisionAliens(Game * game) {
	for (int i = 0; i < 10; i++) {
		// Aliens Collision with Player.
		if (game->enemy_aliens[i]->is_visible && round(game->enemy_aliens[i]->x) >= round(game->player_spaceship->x) && round(game->enemy_aliens[i]->x) <= (round(game->player_spaceship->x) + game->player_spaceship->width - 1) && round(game->enemy_aliens[i]->y) == game->player_spaceship->y) {
			game->enemy_aliens[i]->is_visible = false;
			game->enemy_alive -= 1;
			game->player_lives -= 1;
			RespawnPlayer(game);
		}

		// Aliens Collision with Player Sheild.
		if (game->enemy_aliens[i]->is_visible && game->player_sheild->is_visible && round(game->enemy_aliens[i]->x) >= game->player_sheild->x && round(game->enemy_aliens[i]->x) <= (game->player_sheild->x + game->player_sheild->width - 1) && round(game->enemy_aliens[i]->y) == game->player_sheild->y) {
			game->player_sheild->is_visible = false;
			game->enemy_aliens[i]->is_visible = false;
			game->enemy_alive -= 1;
		}
	}
}

void CollisionBombs(Game * game) {
	for (int i = 0; i < 4; i++) {
		// Bombs collision with Spaceship.
		if (game->enemy_bombs[i]->is_visible && round(game->enemy_bombs[i]->x) >= game->player_spaceship->x && round(game->enemy_bombs[i]->x) <= (game->player_spaceship->x + game->player_spaceship->width - 1) && round(game->enemy_bombs[i]->y) == game->player_spaceship->y) {
			game->enemy_bombs[i]->is_visible = false;
			game->enemy_alive -= 1;
			game->player_lives -= 1;
			RespawnPlayer(game);
		}

		// Bombs collision with Bottom of Screen.
		if (game->enemy_bombs[i]->is_visible && game->enemy_bombs[i]->y > screen_height() - 4) {
			game->enemy_bombs[i]->is_visible = false;
			game->enemy_alive -= 1;
		}

		// Bombs collision with Player Sheild.
		if (game->enemy_bombs[i]->is_visible && game->player_sheild->is_visible && round(game->enemy_bombs[i]->x) >= game->player_sheild->x && round(game->enemy_bombs[i]->x) <= (game->player_sheild->x + game->player_sheild->width - 1) && round(game->enemy_bombs[i]->y) == game->player_sheild->y) {
			game->player_sheild->is_visible = false;
			game->enemy_bombs[i]->is_visible = false;
			game->enemy_alive -= 1;
		}
	}
}

void CollisionBonuses(Game * game) {
	// Collision with Sheild Bonus Drop.
	if (game->bonus_sheild->is_visible && round(game->bonus_sheild->x) >= game->player_spaceship->x && round(game->bonus_sheild->x) <= (game->player_spaceship->x + game->player_spaceship->width - 1) && round(game->bonus_sheild->y) == game->player_spaceship->y) {
		game->player_sheild->is_visible = true;
		game->bonus_sheild->is_visible = false;
	}

	// Collision with Missile Bonus Drop.
	if (game->bonus_missiles->is_visible && round(game->bonus_missiles->x) >= game->player_spaceship->x && round(game->bonus_missiles->x) <= (game->player_spaceship->x + game->player_spaceship->width - 1) && round(game->bonus_missiles->y) == game->player_spaceship->y) {
		game->bonusMissiles += 10;
		game->bonus_missiles->is_visible = false;
	}

	// Collision with Bottom of Screen.
	if (game->bonus_sheild->is_visible && game->bonus_sheild->y > screen_height() - 4) {
		game->bonus_sheild->is_visible = false;
	}
	if (game->bonus_missiles->is_visible && game->bonus_missiles->y > screen_height() - 4) {
		game->bonus_missiles->is_visible = false;
	}
}

void UpdateScreen(Game * game) {
	// Draw the Player: Sheild Bonus
	draw_sprite(game->player_sheild);

	// Draw the Player: Spaceship
	draw_sprite(game->player_spaceship);

	// Draw the Player: Missiles
	for (int i = 0; i < 5; i++) { draw_sprite(game->player_missile[i]); }

	// Draw the Enemy: Aliens
	for (int i = 0; i < 10; i++) { draw_sprite(game->enemy_aliens[i]); }

	//  Draw the Enemy: Bombs
	for (int i = 0; i < 4; i++) { draw_sprite(game->enemy_bombs[i]); }

	// Draw the Bonus Drops.
	draw_sprite(game->bonus_sheild);
	draw_sprite(game->bonus_missiles);


	// Debugging Draws...
}

void GameOver(Game * game) {
	clear_screen();
	draw_string((screen_width()/2)-11,screen_height()/2-2,"----------------------");
    draw_string((screen_width()/2)-11,screen_height()/2-1,"|     You Died :(    |");
    draw_string((screen_width()/2)-11,screen_height()/2,  "|   Press Q to Quit  |");
    draw_string((screen_width()/2)-11,screen_height()/2+1,"| Press R to Restart |");
    draw_string((screen_width()/2)-11,screen_height()/2+2,"----------------------");
	show_screen();
}

// This is my primary Event Loop. This is where most of the fun stuff happens ;)
void Megamaniac() {
    // WTF does this do? Something to do with my Data Structure I assume...
    Game game;

	while (true) {
	    // Let's run our initial setup to get everything for the game ready.
	    SetupGame(&game);

	    // Do we need to change the level? Not really but we should probably start at Level 1.
	    ChangeLevel(&game);

		// This is where the Event Loop actually starts. This will run repeatedly until our game is over.
	    while (!game.game_over) {
		    clear_screen();
		    GameStats(&game);
			int userInput = UserInput(&game);
		    MovementPlayer(&game, userInput);
			MovementMissiles(&game, userInput);
		    MovementAliens(&game);
			MovementBombs(&game);
			MovementBonuses(&game);

			CollisionMissiles(&game);
			CollisionAliens(&game);
		    CollisionBombs(&game);
			CollisionBonuses(&game);

			UpdateScreen(&game);
		    show_screen();
		    timer_pause(25);
	    }

		GameOver(&game);

		int userInput = wait_char();
		while (userInput != 'r' && userInput >= 0) {
			if (userInput == 'q') {
				exit(0);
			}
			if (userInput == 'r') {
				Megamaniac();
			}
			userInput = wait_char();
		}
	}
}

// This is Main, there are many others like it but this is the main one...
int main() {
    setup_screen();
    Megamaniac();
    cleanup_screen();
    return 0;
}
