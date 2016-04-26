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

bool move_bullet( Game * game ) {
	if (!game->bullet->is_visible) {
		return false;
	} else if (!timer_expired(game->bullet_timer)) {
		return false;
	} else {
		game->bullet->x += game->bullet->dx;

		if (game->bullet->x >= screen_width()) {
			game->bullet->is_visible = false;
		}

		for (int i = 0; i < N_ALIENS; i++) {
			if (game->bullet->x == game->aliens[i]->x && game->bullet->y == game->aliens[i]->y) {
				game->bullet->is_visible = false;
				game->aliens[i]->is_visible = false;
				game->score++;
			}
		}

		return true;
	}
}
// ---------------------------------------------------------------- //