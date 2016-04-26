//gcc -std=gnu99 -D_XOPEN_SOURCE=500 -Wall -Werror simple_timer.c -o simple_timer -I../ZDK04 -L../ZDK04 -lzdk04 -lncurses -lm -lrt

#include <stdbool.h>
#include <stdlib.h>
#include "cab202_graphics.h"
#include "cab202_timers.h"

// Structure to hold game data.
typedef struct Game {
    timer_id timer;
    int score;
    bool over;
} Game;

void setup_score( Game * game ) {
    game->score = 0;
    game->over = false;
    game->timer = create_timer(50);
}

bool update_score( Game * game ) {
    if (timer_expired(game->timer)) {
        game->score++;
        if (game->score >= 47) { game->over = true; }
        return true;
    } else {
        return false;
    }
}

void display_score( Game * game ) {
    draw_string(44, 5, "Score: ");
    draw_int(51, 5, game->score);
    draw_formatted(44, 6, "Clock time: %f", get_current_time());
}

// --------- Do not submit the following functions --------- //
void simple_timer( void ) {
    Game game;
    setup_score( &game );
    while ( !game.over ) {
        if ( update_score( &game ) ) {
            clear_screen();
            display_score( &game );
            show_screen();
        }
        timer_pause( 100 );
    }
    timer_pause( 1500 );
}
int main( void ) {
    setup_screen();
    simple_timer();
    cleanup_screen();
    return 0;
}