#include "game.h"
#include <lcd.h>
#include <lcd_lld.h>
#include <color.h>
#include "io.h"

void game_init(game_t* game) {
    //Sysinit

    //gpio init
    io_init();
    
    //uart init

    //lcd init
	LCD_Init();
	LCD_Clear(GUI_COLOR_BLACK);
    
    //struct init
    game->state=prestart;
    game->ticks_per_pixel = SPEED_DEFAULT;
    game->ticks_leftover =0;
} 


void game_step(game_t* game, uint64_t deltaTime) {
    static long l = 0;
    switch(game->state) {
        case prestart:
            //Draw welcome screen
            LCD_DrawRectF(10,10,100,50,GUI_COLOR_BLUE);
            
            //wait on player to press start (host)
            while(!io_button_has_edge(BTN_START));

            //send game start request to slave
            //wait on game accept response
            
            //setup
			//void player_init(player_t* player, uint8_t btn_left, uint8_t btn_right, point_t pos, uint16_t color, direction_t direction); 
            player_init(&(game->player[0]), BTN_PLAYER_1_LEFT, BTN_PLAYER_1_RIGHT, (point_t){.x=10,.y=120}, GUI_COLOR_BLUE, right);
            player_init(&(game->player[1]), BTN_PLAYER_2_LEFT, BTN_PLAYER_2_RIGHT, (point_t){.x=230,.y=120}, GUI_COLOR_RED, left);

            
            //switch state
            game->state = running;
	        LCD_Clear(GUI_COLOR_BLACK);
        break;    
        case running:
        {
			uint16_t ticks; 
			uint16_t pixels = 0;
 
			if(deltaTime) {

				ticks 	= game->ticks_leftover + deltaTime;
				pixels 	= ticks / game->ticks_per_pixel;

				game->ticks_leftover = ticks % game->ticks_per_pixel;  
			}

			for(int i = 0; i < PLAYER_COUNT; i++) {

				bool directionChange = false;
				player_t* player = &(game->player[i]);

				if(io_button_has_edge(player->btn_left)) {
					player->direction= (player->direction + (4 - 1)) % 4 ; // "decrement enum value"
					directionChange = true;
				} else if(io_button_has_edge(player->btn_right)) {
					player->direction= (player->direction + 1) % 4 ; // "increment enum value"
					directionChange = true;
				}

				if(directionChange) {
					player_append_position(player,player->position);
				}
     
				if(pixels) {
					point_t last_point = player->past_positions[player->num_positions-1];  
					switch(player->direction) {
						case down:
							player->position.y+=pixels;
							LCD_DrawRectF(  player->position.x,
											last_point.y,
											PLAYER_WIDTH,
											player->position.y - last_point.y,
											player->color);
							break;
						case left:
							player->position.x-=pixels;
							LCD_DrawRectF(  player->position.x,
											player->position.y,
											last_point.x -player->position.x,
											PLAYER_WIDTH,
											player->color);
							break;
						case up:
							player->position.y-=pixels;
							LCD_DrawRectF(  player->position.x,
											player->position.y,
											PLAYER_WIDTH,
											last_point.y - player->position.y,
											player->color);
							break;
						case right:
							player->position.x+=pixels;
							LCD_DrawRectF(  last_point.x,
											player->position.y,
											player->position.x - last_point.x,
											PLAYER_WIDTH,
											player->color);
							break;
					}
				}
			 }
         break; 
		}
    }
}


