//gcc -std=gnu99 -D_XOPEN_SOURCE=500 -Wall -Werror zombie_wrap.c -o zombie_wrap -I../ZDK04 -L../ZDK04 -lzdk04 -lncurses -lm -lrt

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"

// Structure to hold game data.
typedef struct Game {
    timer_id timer;
    sprite_id zombie;
    int laps;
    bool over;
} Game;

void setup_zombie( Game * game ) {
    game->laps = 0;
    game->over = false;
    game->timer = create_timer(50);
    game->zombie = create_sprite(23, 17, 1, 1, "Z");
    game->zombie->dx = 1.15;
}

bool update_zombie( Game * game ) {
    if (!game->over) {
        if (timer_expired(game->timer)) {
            double posCurrent = game->zombie->x;
            double posUpdated = game->zombie->x = posCurrent + game->zombie->dx;
            if (round(game->zombie->x) > screen_width() - 1) {
                game->zombie->x = 0;
                game->laps++;
                if (game->laps >= 6) { game->over = true;}
            }
            if (posCurrent != posUpdated) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void display_zombie( Game * game ) {
    draw_sprite(game->zombie);
}

// --------- Do not submit the following functions --------- //

void zombie_wrap( void ) {
    Game game;

    setup_zombie( &game );

    while ( !game.over ) {
        if ( update_zombie( &game ) ) {
            clear_screen();
            display_zombie( &game );
            show_screen();
        }

        timer_pause( 25 );
    }

    timer_pause( 1500 );
}

int main( void ) {
    setup_screen();
    zombie_wrap();
    cleanup_screen();
    return 0;
}