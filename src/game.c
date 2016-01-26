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

bool game_check_line_collision(player_t* player, point_t start, point_t end, uint8_t pixels){

    bool line_is_horizontal = (start.y == end.y);
    bool player_is_horizontal = (player->direction == left || player->direction == right);

    if(line_is_horizontal == player_is_horizontal){ // if player moves parallel to the line, there is no point in checking
        return false; // no collision possible because parallel moving
    }

    if(!player_is_horizontal && (player->position.x < min(start.x, end.x) || player->position.x > max(start.x, end.x))){
        return false; // if player is passing by horizontally, no collision possible
    } 
    
    if(player_is_horizontal && (player->position.y < min(start.y, end.y) || player->position.y > max(start.y, end.y))){
        return false; // if player is passing by vertically, no collision possible
    }

    switch(player->direction){
        case up:
               if(player->position.y > start.y && ((int16_t)player->position.y - pixels) <= start.y) {
                   return true;
               }
            break;
        case down:
               if(player->position.y < start.y && ((int16_t)player->position.y + pixels) >= start.y) {
                   return true;
               }
            break;
        case left:
               if(player->position.x > start.x && ((int16_t)player->position.x - pixels) <= start.x) {
                   return true;
               }
            break;
        case right:
               if(player->position.x < start.x && ((int16_t)player->position.x + pixels) >= start.x) {
                   return true;
               }
            break;
    }
    return false;
}

bool game_player_check_collision(game_t* game, player_t* player, uint8_t pixels){
   
    // Check bounding collision
    switch(player->direction){
        case up:
            if((int16_t)(player->position.y) - pixels <= TFT_GAME_FIELD_TOP){
                return true; // Collision at top
            }
            break;
        case down:
            if((int16_t)(player->position.y) + pixels >= (TFT_HEIGHT - TFT_GAME_FIELD_BOTTOM - 1)){
                return true; // Collision at bottom
            }
            break;
        case left:
            if((int16_t)(player->position.x) - pixels <= TFT_GAME_FIELD_LEFT){
                return true; // Collision at left
            }
            break;
        case right:
            if((int16_t)(player->position.x) + pixels >= (TFT_WIDTH - TFT_GAME_FIELD_RIGHT - 1)){
                return true; // Collision at right
            }
            break;
    }

    // Check collision with players (including self)
    for(int i = 0; i < PLAYER_COUNT; i++){

        player_t* colliding = &(game->player[i]); // pointer to player whose lines we want to check (against opponent or self)
        point_t last_point = colliding->past_positions[0]; // start point of the line 

        // For each line segment (last_point to curr_point)
        for(int j = 1; j < colliding->num_positions; j++ ){
            point_t curr_point = colliding->past_positions[j]; // end point of the line 
            
            if(game_check_line_collision(player, last_point, curr_point, pixels)){ // check if player collides with line segment
                return true;
            }

            last_point = curr_point; // set new start point 
        }

        if( player != colliding && // do not check against yourself  
            game_check_line_collision(player, last_point, colliding->position, pixels)){ // check if player collides with newest segment of opponent
            return true;
        }
    }

    return false; // no collision!
}

void game_player_update(game_t* game, player_t* player, uint8_t pixels){

    bool directionChange = false;

    // Check for button presses
    if(io_button_has_edge(player->btn_left)) {
        player->direction= (player->direction + (4 - 1)) % 4 ; // "decrement enum value"
        directionChange = true;
    } else if(io_button_has_edge(player->btn_right)) {
        player->direction= (player->direction + 1) % 4 ; // "increment enum value"
        directionChange = true;
    }

    // Check if player is alive
    if(player->state != alive){
        return;
    }
    // Change direction
    if(directionChange) {
        player_append_position(player,player->position);
    }

    if(pixels) {

        // Check if a collision is about to happen
        if(game_player_check_collision(game, player, pixels)){
            player->state=dead;
        }

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

bool game_step(game_t* game, uint64_t deltaTime) {
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
            player_init(&(game->player[0]), 
                        BTN_PLAYER_1_LEFT, 
                        BTN_PLAYER_1_RIGHT,
                        (point_t){
                            .x=(TFT_GAME_FIELD_START_OFFSET + TFT_GAME_FIELD_LEFT),
                            .y=(((TFT_HEIGHT - TFT_GAME_FIELD_TOP - TFT_GAME_FIELD_BOTTOM) / 2) + TFT_GAME_FIELD_TOP)
                        }, 
                        GUI_COLOR_BLUE, 
                        right);

            player_init(&(game->player[1]),
                        BTN_PLAYER_2_LEFT, 
                        BTN_PLAYER_2_RIGHT, 
                        (point_t){
                            .x=(TFT_WIDTH - 1 - TFT_GAME_FIELD_RIGHT - TFT_GAME_FIELD_START_OFFSET),
                            .y=(((TFT_HEIGHT - TFT_GAME_FIELD_TOP - TFT_GAME_FIELD_BOTTOM) / 2) + TFT_GAME_FIELD_TOP)
                        }, 
                        GUI_COLOR_RED, 
                        left);
            
            //switch state
            game->state = running;

	        LCD_Clear(GUI_COLOR_BLACK);
            LCD_DrawRect(TFT_GAME_FIELD_LEFT, 
                        TFT_GAME_FIELD_TOP, 
                        (TFT_WIDTH - TFT_GAME_FIELD_LEFT - TFT_GAME_FIELD_RIGHT - 1), 
                        (TFT_HEIGHT - TFT_GAME_FIELD_TOP - TFT_GAME_FIELD_BOTTOM - 1), 
                        GUI_COLOR_WHITE);
            return true;

        case running:
        {
			uint16_t ticks; 
			uint16_t pixels = 0;
 
			if(deltaTime) {
				ticks 	= game->ticks_leftover + deltaTime;
				pixels 	= ticks / game->ticks_per_pixel;
				game->ticks_leftover = ticks % game->ticks_per_pixel;  
			}

            // For each player do ...
            bool all_players_dead = true;
			for(int i = 0; i < PLAYER_COUNT; i++) {
                player_t* player = &(game->player[i]);
                game_player_update(game, player, pixels);
                if(player->state!=dead) {
                    all_players_dead=false;
                }
			}

            if(all_players_dead) {
                game->state=ended;
	            LCD_Clear(GUI_COLOR_BLACK);
                return true;
            } else {
                return false;
            }
		}

        case ended:

        game->state= prestart;
        return false;
    }
}


