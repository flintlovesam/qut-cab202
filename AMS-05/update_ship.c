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

bool update_ship( Game * game, int key_code ) {
	int shipX = round(game->ship->x);
	int shipY = round(game->ship->y);

	if (key_code == '1' && shipX > 0) {
		game->ship->x--;
	} else if (key_code == '2' && (shipY + game->ship->height) < (screen_height() - 2)) {
		game->ship->y++;
	} else if (key_code == '3' && (shipX + game->ship->width) < 30) {
		game->ship->x++;
	} else if (key_code == '5' && shipY > 0) {
		game->ship->y--;
	} else {
		return false;
	}

	int shipXnew = round(game->ship->x);
	int shipYnew = round(game->ship->y);

	if (shipX != shipXnew) {
		return true;
	} else if (shipY != shipYnew) {
		return true;
	}
	return false;
}

// ---------------------------------------------------------------- //
