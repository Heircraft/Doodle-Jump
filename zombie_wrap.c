#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"
//#include "Week5Ex2.h"
#define STEP_CHANGE 1

// Structure to hold game data.
typedef struct Game {
	timer_id timer;
	int laps;
	bool over;
} Game;
sprite_id zombie[2];


void setup_zombie( Game * game) {
	game->laps = 0;
	game->over = false;
	game->timer = create_timer(70);
	
	zombie[0] = sprite_create(screen_width() / 2,screen_height() / 2, 4, 4, "ZZZZ  Z  Z  ZZZZ");
	
	for(int i = 0; i < 3; i ++) {
		zombie[i] = sprite_create(screen_width() / 2,screen_height() / 2, 4, 4, "ZZZZ  Z  Z  ZZZZ");

		if (zombie[i]->y == zombie[i-1]->y) {
		zombie[i]->y  = zombie[i]->y  + 10;
		}
			zombie[i]->dx = 1.24;
		// if (zombie[i]->y >= zombie[i-1]->y) {
			// zombie[i]->x = zombie[i]->x - 9;
		// }
	}
	
	
}

bool update_zombie( Game * game ) {
	if(game->laps >= 3) {
		return false;
	}
	if ( timer_expired( game->timer ) ) {
		sprite_step(zombie[0]);
		for(int i = 0; i < 3; i ++) {
			sprite_step(zombie[i]);
		
		
		if ( zombie[i]->x >= screen_width() - 1 ) {
			zombie[i]->x = 0;
			game->laps++;
			if(game->laps >=3) {
				game->over = true;
			}
		}
		}
		return true;
	}
	else {
		return false;
	}
}

void display_zombie( Game * game ) {
	
	for(int i = 0; i < 3; i ++) {
		sprite_draw(zombie[i]);
	}
}

// --------- Do not submit the following functions --------- // 

void zombie_wrap( void ) {
	Game game;

	setup_zombie( &game );
	clear_screen();
	display_zombie( &game );
	show_screen();

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