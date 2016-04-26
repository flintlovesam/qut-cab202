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

void launch_bullet( Game * game ) {
	if (game->bullet->is_visible) {
		return;
	} else {
		game->bullet->x = game->ship->x + game->ship->width;
		game->bullet->y = game->ship->y + (game->ship->height / 2);

		game->bullet->dx = 1;

		game->bullet->is_visible = true;

		reset_timer(game->bullet_timer);
	}
}

// ---------------------------------------------------------------- //
