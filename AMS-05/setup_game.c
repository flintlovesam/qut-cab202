#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"

// ---------------------------------------------------------------- //

#include "game.h"

// ---------------------------------------------------------------- //

void setup_game( Game * game ) {
    game->score = 0;
	game->over = false;
	game->event_loop_delay = 10;
}

// ---------------------------------------------------------------- //

void setup_ship( Game * game ) {
	static char bitmap[] =
		"======  _  "
		"   \\   / \\ "
		"  ====| o |"
		"   /   \\_/ "
		"======     ";

    game->ship = create_sprite(3, 4, 11, 5, bitmap);
}

// ---------------------------------------------------------------- //

void setup_bullet( Game * game ) {
    static char bitmap[] = { '-' };

    game->bullet = create_sprite(0, 0, 1, 1, bitmap);
	game->bullet->is_visible = false;
	game->bullet_timer = create_timer(30);
}

// ---------------------------------------------------------------- //

void setup_aliens( Game * game, int rock_x[], int rock_y[] ) {
    static char bitmap[] = { '@' };

	for (int i = 0; i < N_ALIENS; i++) {
		game->aliens[i] = create_sprite(rock_x[i], rock_y[i], 1, 1, bitmap);
		game->aliens[i]->dy = 1;
	}

	game->alien_timer = create_timer(350);
}

// ---------------------------------------------------------------- //
