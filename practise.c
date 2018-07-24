#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "cab202_timers.h"


//-----------------
//resize terminal
//-----------------
//printf '\e[8;55;70t'

//---------------------
//platform structure
//---------------------
typedef struct Game{
	int c;
	timer_id playing_time;
	timer_id platform_timer;
	int level;
	int score;
	sprite_id character;
	sprite_id platform[1000];
	sprite_id splatform;
	int tc;
	int minutes;
} Game;

//global variables
int gameovercounter;
int lives;
bool died;
char * badplatform = "xxxxxxxxxxxxxx";
static char *bitmap =
		"|-------------|"
		"|GAME     OVER|"
		"|-------------|";

//--------------
//set up game
//--------------
void first_time_setup(Game * game) {
	gameovercounter = 0;
	game->score = 0;
	game->platform_timer = create_timer(100);
	lives = 3;
	game->playing_time = create_timer(1000);
	game->tc = 0;
	game->minutes = 0;
}

//--------------
//set up game
//--------------
void setup_game(Game *game) {
	game->score -= 1;
	died = false;
	timer_reset(game->playing_time);
	game->level = 1;
	game->character = sprite_create(screen_width() / 2, screen_height()- 8,1,3, "0|^");
	game->character->dy = -1.0;
	game->splatform = sprite_create(screen_width() / 2 - 3, screen_height() - 5, 7, 2, "==============");
	game->splatform->dy = -1.0;
	
	time_t now = time( NULL );
	srand( now );
	for (int i =0; i < 1000; i++) {
	
		int g_or_b = rand() % (20-10) + 10;
		if (g_or_b <= 12) {
			game->platform[i] = sprite_create(rand() % screen_width() - 6,  screen_height() - 1, 7, 2, badplatform);		
		} else if (g_or_b > 12) {
			game->platform[i] = sprite_create(rand() % screen_width() - 6,  screen_height()  - 1, 7, 2, "==============");
		}
		
		//move if close y
		if (game->platform[i]->y <= game->platform[i-1]->y) {
			game->platform[i]->y = game->platform[i-1]->y + 5;
		}
		
		//if greater then half way make a random number between 0 and previous platform x - 11
		if (game->platform[i-1]->x > screen_width() / 2) {
			int arse = (game->platform[i-1]->x - 11) + 0;
			int n = rand() % arse;
			game->platform[i]->x = n;
		}
		
		//if less then half way make a random number between previous x + 11 and screen-width - 7
		if (game->platform[i-1]->x <= screen_width() / 2) {
			int n = rand() % (screen_width() - 7) + (game->platform[i-1]->x + 11);
			game->platform[i]->x = n;
		}

		// if off screen come back
		if (game->platform[i]->x < 0) {
			game->platform[i]->x = 0 ;
		}
		if (game->platform[i]->x > screen_width() - 8) {
			game->platform[i]->x = screen_width() - 9;
		}
		game->platform[i]->dy = -1.0;
	}
}

//----------------------------------------------
//draw things to the screen
//----------------------------------------------
 void display_game(Game *game) {
	sprite_draw(game->character);
	sprite_draw(game->splatform);
	
	for (int i = 0; i < 1000; i++) {
			sprite_draw(game->platform[i]);
	}
	
	//use timer expired to create 1 second effect
	if (timer_expired(game->playing_time)) {
		game->tc += 1;
	} else if (game->tc == 60) {
		game->tc = 0;
		game->minutes = 1;
	}
	//draw playing time to screen
	draw_string(screen_width() - 20,1,"Playing Time:");
	draw_char(screen_width() - 5,1,':');
	draw_int(screen_width()- 4,1, game->tc);
	draw_int(screen_width()-6,1, game->minutes);
	draw_int(screen_width()-6,1, game->minutes);
	
	//draw score level and lives to screen
	draw_formatted(1, screen_height()-1 - 1,"Score: %d", game->score);
	draw_formatted(13, screen_height()-1 - 1,"Level: %d",game->level);
	draw_formatted(1,1,"Lives: %d", lives);
	
	// displays borders
	draw_line( 1, 2, screen_width()-1 - 1, 2, '-');
	draw_line( 1, screen_height()-1 - 2, screen_width()-1 - 1, screen_height()-1 - 2, '-');
	draw_line( 1, 0, screen_width()-1 - 1, 0, '-');
	draw_line( 1, screen_height()-1 , screen_width()-1 - 1, screen_height()-1 , '-');
	draw_char(0,0,'/');
	draw_char(screen_width()-1,0,'\\');
	draw_char(0,2,'\\');
	draw_char(0,screen_height()-1 - 2,'/');
	draw_char(screen_width()-1,2,'/');
	draw_char(0,screen_height()-1,'\\');
	draw_char(screen_width()-1 ,screen_height()-1 - 2,'\\');
	draw_char(screen_width()-1,screen_height()-1,'/');
	draw_char(0,1,'|');
	draw_char(0,screen_height()-1 - 1,'|');
	draw_char(screen_width()-1,1,'|');
	draw_char(screen_width()-1,screen_height()-1 - 1,'|');
 }

//---------------------------------------
//updates the game and returns true or false
//---------------------------------------
bool update_game(Game * game) {
	
	if (timer_expired(game->platform_timer)) {
		sprite_step(game->splatform);
		
		for (int i = 0; i < 1000; i++) {
			
			//if inside platform stop being retarded 
		if (game->platform[i]->y == game->character->y  + 2&&game->character->x >= game->platform[i]->x && game->character->x <= game->platform[i]->x + 6) {
			game->character->y -= 1;
		}
			
		// make platforms invisible when on scoreboard
		if (game->platform[i]->y < screen_height() - 3) {
		game->platform[i]->is_visible = true;
		} 
		if (game->platform[i]->y >= screen_height() - 3) {
			game->platform[i]->is_visible = false;
		} else if (game->platform[i]->y <= 3) {
			game->platform[i]->is_visible = false;
		}
		
		//move platforms up one
		sprite_step(game->platform[i]);
		}

		// if character goes outside boundaries lose life
		if(game->character->y >= screen_height()- 6) {
			lives -= 1;
			died = true;
		} else if (game->character->y <= 3) {
			lives -= 1;
			game->score+= 1;
			died = true;
		}

		// if off left or right come back one
		if (game->character->x > screen_width()-1) {
			game->character->x -= 1;
		} else if (game->character->x < 0) {
			game->character->x += 1;
		}
		return true;
	}
	return false;
}
//----------------------------------
//make character move up
//----------------------------------
void stay_on_platform(Game * game) {
	
	//increment score
	if (game->c++ == 1) {
		game->score += 1;
	}
	game->character->y -= 1;
}
//----------------------------------
//make character fall
//----------------------------------
void apply_gravity(Game * game) {
	game->character->y += 1;
	game->c = 0;
}
//----------------------------------
//if character is touching platform then...
//----------------------------------
bool is_touching_platform(Game * game) {
	for (int i = 0; i < 1000; i++) {

		//die on bad platform
		if (game->platform[i]->y >= game->character->y && game->platform[i]->y < game->character->y + 3 &&game->character->x >= game->platform[i]->x && game->character->x <= game->platform[i]->x + 6 && game->platform[i]->bitmap == badplatform) {
			lives -= 1;
			died = true;
		}
		
		// make character stop on platform
		if (game->platform[i]->y >= game->character->y && game->platform[i]->y < game->character->y + 3 &&game->character->x >= game->platform[i]->x && game->character->x <= game->platform[i]->x + 6) {
			return true;
		} else if (game->character->y + 2== game->splatform->y && game->character->x >= game->splatform->x && game->character->x <= game->splatform->x + 6) {
			return true;
		}
	}
		return false;
}

//----------------------------------
//where the game is processed
//----------------------------------
bool process_key(Game * game) {
	int key = get_char();

	// Remember original position
	int cx_0 = game->character->x;

	if ( key == KEY_LEFT ) {
		game->character->x = cx_0 - 1;
	}
	else if ( key == KEY_RIGHT) {
		game->character->x = cx_0 + 1;
	}
	int ncx_0 = game->character->x;
	if (cx_0 != ncx_0) {
		return true;
	}
	return false;
}

//----------------------------------
//restart or quit
//----------------------------------
bool main_menu(Game * game) {
	int key = get_char();
	
	char * intro =
	"|  |  |  |---   |      |---   |---|   |---|---|   |---"
	"|  |  |  |---   |      |      |   |   |   |   |   |---"
	"|--|--|  |---   |---   |---   |---|   |   |   |   |---";
	
	sprite_id welcome = sprite_create(screen_width() / 2 - 28, 4,54,3,intro);
	sprite_draw(welcome);
	draw_string(screen_width() / 3 - 3, screen_height() / 3 + 2, "PRESS 'L' TO CHANGE LEVELS");
	draw_string(screen_width() / 3 , screen_height() / 3, "PRESS 'R' TO START");
	draw_string(screen_width() / 3, screen_height()  - 6, "PRESS 'Q' TO QUIT");\
	if (game->level == 0) {
		game->level = 1;
	}
	draw_formatted(screen_width() / 3 + 6, screen_height() / 3 + 3,"LEVEL: %d", game->level);
	
	if ( key == 'q' ) {
		exit(0);
	} else if (key == 'l'){
		game->level += 1;
		if (game->level == 2) {
			game->level = 1;
		}
	}else if ( key == 'r' ) {
		return true;
	}
	return false;
}

//---------------------
//displays animation for game over screen
//---------------------
void display_gameover1(Game * game) {
	
	sprite_id game__over11 = sprite_create(screen_width() / 2 - 10, screen_height() / 2,15,3,bitmap);
	sprite_id game__over12 = sprite_create(screen_width() / 2 - 25, screen_height() / 2 + 4,15,3,bitmap);
	sprite_id game__over13 = sprite_create(screen_width() / 2 + 5, screen_height() / 2 + 4,15,3,bitmap);
	sprite_id game__over14 = sprite_create(screen_width() / 2 - 25, screen_height() / 2 - 4,15,3,bitmap);
	sprite_id game__over15 = sprite_create(screen_width() / 2 + 5, screen_height() / 2 - 4,15,3,bitmap);
	draw_formatted(screen_width() / 3, screen_height() / 3 - 1,"YOUR SCORE WAS %d", game->score);
	sprite_draw(game__over11);
	sprite_draw(game__over12);
	sprite_draw(game__over13);
	sprite_draw(game__over14);
	sprite_draw(game__over15);
}

//---------------------
//displays animation for game over screen
//---------------------
void display_gameover2(Game * game) {
	
	sprite_id game__over21 = sprite_create(screen_width() / 2 - 25, screen_height() / 2,15,3,bitmap);
	sprite_id game__over22 = sprite_create(screen_width() / 2 + 5, screen_height() / 2,15,3,bitmap);
	sprite_id game__over23 = sprite_create(screen_width() / 2 - 10, screen_height() / 2 + 4,15,3,bitmap);
	sprite_id game__over24 = sprite_create(screen_width() / 2 - 10, screen_height() / 2 - 4,15,3,bitmap);
	draw_formatted(screen_width() / 3, screen_height() / 3 - 1,"YOUR SCORE WAS %d", game->score);
	sprite_draw(game__over21);
	sprite_draw(game__over22);
	sprite_draw(game__over23);
	sprite_draw(game__over24);
}
//----------------------------------
//where the game is processed
//----------------------------------
void event_loop(void) {
	
	Game game;
	while(true) {
		while(main_menu(&game) != true) {
				main_menu(&game);
				show_screen();
		}
		first_time_setup(&game);
	
		while(lives > -1) {
			setup_game( &game );
			clear_screen();
			display_game( &game );

			while (died != true ) {
				if ( update_game( &game )) {
					if (is_touching_platform(&game)) {
						stay_on_platform(&game);
						process_key(&game);
					} else {
						apply_gravity(&game);
					}
					clear_screen();
					display_game( &game );
					show_screen();
				 } 
			}
		}
		clear_screen();
		while(gameovercounter < 3) {
				display_gameover1(&game) ;
				show_screen();
				timer_pause(500);
				clear_screen();
				display_gameover2(&game) ;
				show_screen();
				timer_pause(500);
				clear_screen();
				gameovercounter += 1;
		}
	}
}


int main(void) {
setup_screen();
event_loop();
cleanup_screen();
return 0;
}