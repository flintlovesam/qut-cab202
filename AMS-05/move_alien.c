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

bool move_alien( sprite_id alien ) {
	if (!alien->is_visible) {
		return false;
	} else {
		int startY = alien->y;
		alien->y += alien->dy;
		int endY = alien->y;

		if (endY >= screen_height() - 2) {
			alien->y = 0;
		}

		if (startY != endY) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------- //
