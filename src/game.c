#include "game.h"
#include <lcd.h>
#include <lcd_lld.h>
#include <color.h>
#include "io.h"

static pin_t pin_left;
static pin_t pin_right;


void game_init(game_t* game) {
    //Sysinit

    //gpio init (move to module?)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE);
    pin_create(&pin_right, GPIOC, 7, true);
    pin_create(&pin_left, GPIOI, 0, true);
    
    //uart init

    //lcd init
	LCD_Init();
	LCD_Clear(GUI_COLOR_BLACK);
    
    //struct init
    game->state=prestart;
    game->host_id = 0;
    game->ticks_per_pixel = SPEED_SLOW;
    game->ticks_leftover =0;
} 


void game_step(game_t* game, uint64_t deltaTime) {
    static long l = 0;
    switch(game->state) {
        case prestart:
            //Draw welcome screen
            LCD_DrawRectF(10,10,100,50,GUI_COLOR_BLUE);
            
            //wait on player to press start (host)
            while(!pin_get(&pin_right));
            while(pin_get(&pin_right));

            //send game start request to slave
            //wait on game accept response
            
            //setup
            player_init(&(game->player[game->host_id]));

            
            //switch state
            game->state = running;
	        LCD_Clear(GUI_COLOR_BLACK);
        break;    
        case running:
        {
            bool directionChange = false;
            player_t* host_player = &(game->player[game->host_id]);
            if(pin_get(&pin_left)) {
                while(pin_get(&pin_left));
                host_player->direction= (host_player->direction + 3) % 4 ; // "decrement enum value"
                directionChange = true;
            } else if(pin_get(&pin_right)) {
                while(pin_get(&pin_right));
                host_player->direction= (host_player->direction + 1) % 4 ; // "increment enum value"
                directionChange = true;
            }

            if(directionChange) {
                player_append_position(host_player,host_player->position);
            }
            if(deltaTime) {
                uint16_t ticks = game->ticks_leftover + deltaTime;
                uint16_t pixels = ticks / game->ticks_per_pixel;
                game->ticks_leftover = ticks % game->ticks_per_pixel;       
                if(pixels) {
                    point_t last_point = host_player->past_positions[host_player->num_positions-1];  
                    switch(host_player->direction) {
                        case down:
                            host_player->position.y+=pixels;
                            LCD_DrawRectF(  host_player->position.x,
                                            last_point.y,
                                            PLAYER_WIDTH,
                                            host_player->position.y - last_point.y,
                                            host_player->color);
                            break;
                        case left:
                            host_player->position.x-=pixels;
                            LCD_DrawRectF(  host_player->position.x,
                                            host_player->position.y,
                                            last_point.x -host_player->position.x,
                                            PLAYER_WIDTH,
                                            host_player->color);
                            break;
                        case up:
                            host_player->position.y-=pixels;
                            LCD_DrawRectF(  host_player->position.x,
                                            host_player->position.y,
                                            PLAYER_WIDTH,
                                            last_point.y - host_player->position.y,
                                            host_player->color);
                            break;
                        case right:
                            host_player->position.x+=pixels;
                            LCD_DrawRectF(  last_point.x,
                                            host_player->position.y,
                                            host_player->position.x - last_point.x,
                                            PLAYER_WIDTH,
                                            host_player->color);
                            break;
                    }
                }
            }
         }
         break; 
    }
}


