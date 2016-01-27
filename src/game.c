#include <lcd.h>
#include <lcd_lld.h>
#include <color.h>
#include <stdio.h>

#include "game.h"
#include "io.h"

void game_init(game_t* game, uint16_t ticks_per_sec) {
    
    //gpio init
    io_init();
    
    //lcd init
    LCD_Init();
    LCD_Clear(GUI_COLOR_BLACK);
    
    //struct init
    game->state=prestart;
    game->ticks_per_pixel = SPEED_DEFAULT;
    game->ticks_leftover = 0;
    game->ticks_per_sec = ticks_per_sec;
    game->time = 0;
    game->ticks_sum_sec = 0;
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
                   return true; // going up and hitting a line segment
               }
            break;
        case down:
               if(player->position.y < start.y && ((int16_t)player->position.y + pixels) >= start.y) {
                   return true; // going down and hitting a line segment
               }
            break;
        case left:
               if(player->position.x > start.x && ((int16_t)player->position.x - pixels) <= start.x) {
                   return true; // going left and hitting a line segment
               }
            break;
        case right:
               if(player->position.x < start.x && ((int16_t)player->position.x + pixels) >= start.x) {
                   return true; // going right and hitting a line segment
               }
            break;
    }
    return false;
}

bool game_check_boundary_collision(game_t* game, player_t* player, uint8_t pixels){
    
    // Check boundary collision
    switch(player->direction){
        case up:
            if((int16_t)(player->position.y) - pixels <= TFT_GAME_FIELD_TOP){
                return true; // Collision at top boundary
            }
            break;
        case down:
            if((int16_t)(player->position.y) + pixels >= (TFT_HEIGHT - TFT_GAME_FIELD_BOTTOM - 1)){
                return true; // Collision at bottom boundary
            }
            break;
        case left:
            if((int16_t)(player->position.x) - pixels <= TFT_GAME_FIELD_LEFT){
                return true; // Collision at left boundary
            }
            break;
        case right:
            if((int16_t)(player->position.x) + pixels >= (TFT_WIDTH - TFT_GAME_FIELD_RIGHT - 1)){
                return true; // Collision at right boundary
            }
            break;
    }

    return false;
}

bool game_check_player_collision(game_t* game, player_t* player, uint8_t pixels){
    // Check for collisions with players (including self)
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
}

bool game_check_collision(game_t* game, player_t* player, uint8_t pixels){ // Check boundary and player collisions

    // Check for collisions with boundarys
    if(game_check_boundary_collision(game, player, pixels)){
        return true;
    }
    
    // Check for collisions with players (including self)
    if(game_check_player_collision(game, player, pixels)){
        return true;
    }

    return false; // no collision!
}

bool game_player_update(game_t* game, player_t* player, uint8_t pixels){

    bool direction_change = false;
    bool state_changed = false;

    // Check for button presses
    if(io_button_has_edge(player->btn_left)) { // If left button is pressed
        player->direction= (player->direction + (4 - 1)) % 4 ; // Decrement direction value (counterclockwise)
        direction_change = true;
    } else if(io_button_has_edge(player->btn_right)) { // If right button is pressed
        player->direction= (player->direction + 1) % 4 ; // Increment direction value (clockwise)
        direction_change = true;
    }

    // Check if player is alive
    if(player->state != alive){
        return state_changed; // If player is dead return state
    }
    // Change direction
    if(direction_change) {
        player_append_position(player,player->position); // Append new position if direction has changed
    }

    if(pixels) {
        if(game_check_collision(game, player, pixels)){ // Check if a collision is about to happen
            player->state = dead; // If a collision is happening kill the player
            state_changed = true; // return the state
        }

        point_t last_point = player->past_positions[player->num_positions-1]; // Get the players newest point

        switch(player->direction) { // Get the players moving direction and render his move
            case down: // render down
                player->position.y+=pixels;
                LCD_DrawRectF(  player->position.x,
                                last_point.y,
                                PLAYER_WIDTH,
                                player->position.y - last_point.y,
                                player->color);
                break;
            case left: // render left
                player->position.x-=pixels;
                LCD_DrawRectF(  player->position.x,
                                player->position.y,
                                last_point.x -player->position.x,
                                PLAYER_WIDTH,
                                player->color);
                break;
            case up: // render up
                player->position.y-=pixels;
                LCD_DrawRectF(  player->position.x,
                                player->position.y,
                                PLAYER_WIDTH,
                                last_point.y - player->position.y,
                                player->color);
                break;
            case right: // render right
                player->position.x+=pixels;
                LCD_DrawRectF(  last_point.x,
                                player->position.y,
                                player->position.x - last_point.x,
                                PLAYER_WIDTH,
                                player->color);
                break;
        }
    }

    return state_changed; // return state
}

bool game_step(game_t* game, uint64_t delta_time) { // Calculate the next game step
    static long l = 0;

    switch(game->state) {
        case prestart: // If the game is in prestart state
            // Draw welcome screen
            LCD_DrawRectF(10,10,100,50,GUI_COLOR_BLUE);
            
            // Wait on player to press start
            while(!io_button_has_edge(BTN_START));

            // Setup the two players
            player_init(&(game->player[0]), // Player object to fill
                        BTN_PLAYER_1_LEFT,  // Left-button
                        BTN_PLAYER_1_RIGHT, // Right-button
                        (point_t){
                            .x=(TFT_GAME_FIELD_START_OFFSET + TFT_GAME_FIELD_LEFT), // x start coordinate
                            .y=(((TFT_HEIGHT - TFT_GAME_FIELD_TOP - TFT_GAME_FIELD_BOTTOM) / 2) + TFT_GAME_FIELD_TOP) // y start coordinate
                        }, 
                        GUI_COLOR_BLUE, // color
                        right); // default moving direction

            player_init(&(game->player[1]), // Player object to fill
                        BTN_PLAYER_2_LEFT,  // Left-button
                        BTN_PLAYER_2_RIGHT, // Right-button
                        (point_t){
                            .x=(TFT_WIDTH - 1 - TFT_GAME_FIELD_RIGHT - TFT_GAME_FIELD_START_OFFSET), // x start coordinate
                            .y=(((TFT_HEIGHT - TFT_GAME_FIELD_TOP - TFT_GAME_FIELD_BOTTOM) / 2) + TFT_GAME_FIELD_TOP) // y start coordinate
                        }, 
                        GUI_COLOR_RED, // color
                        left); // default moving direction
            
            game->state = running; // Switch the game state to running
            game->time = 0; // Reset the game time

            LCD_Clear(GUI_COLOR_BLACK); // Clear the background

            // Draw the game boundary
            LCD_DrawRect(TFT_GAME_FIELD_LEFT, // left top x
                        TFT_GAME_FIELD_TOP, // left top y
                        (TFT_WIDTH - TFT_GAME_FIELD_LEFT - TFT_GAME_FIELD_RIGHT - 1), // right bottom x
                        (TFT_HEIGHT - TFT_GAME_FIELD_TOP - TFT_GAME_FIELD_BOTTOM - 1), // right bottom y
                        GUI_COLOR_WHITE); // Color of the boundary

            LCD_SetTextColor(GUI_COLOR_WHITE); // Reset color to white
            LCD_DisplayStringXY(TFT_GAME_HEADER_TIME_X, TFT_GAME_HEADER_TIME_Y, "Time: 0:00"); // Draw the zero-time

            for(int i = 0; i < PLAYER_COUNT; i++){ // For each player print its name and its state
                static char buf[16]; // Text buffer
                LCD_SetTextColor(game->player[i].color); // Set the text color according to the players color
                sprintf(buf, "Player%d: alive", (i+1)); // Print the state and the players name to the text buffer
                LCD_DisplayStringXY(TFT_GAME_HEADER_PLAYER_X+i*TFT_GAME_HEADER_PLAYER_WIDTH, TFT_GAME_HEADER_PLAYER_Y, buf); // Print everything
            }

            return true;

        case running:
        {
            uint16_t ticks;
            uint16_t pixels = 0;
 
            if(delta_time) {
                ticks = game->ticks_leftover + delta_time; // Calculate the number of past ticks
                pixels = ticks / game->ticks_per_pixel; // Calculate the number of pixels moved in the calculated amount of ticks
                game->ticks_leftover = ticks % game->ticks_per_pixel; // Calculate the number of ticks which are left
                game->ticks_sum_sec += delta_time; // Add the delta_time to the tick sum which is used to calculate the game time

                uint16_t new_seconds = game->ticks_sum_sec / game->ticks_per_sec; // Calculate number of seconds from past ticks

                game->time += new_seconds;  // Add the new amount of seconds to the game time
                game->ticks_sum_sec = game->ticks_sum_sec % game->ticks_per_sec; // Limit the tick sum used to calculate the amount of seconds

                if(new_seconds > 0){    // Print the time if it got updated
                    static char buf[15]; // Textbufer
                    sprintf(buf, "Time: %d:%02d", (game->time / 60), (game->time % 60)); // Format time and paste it to the textbuffer
                    LCD_SetTextColor(GUI_COLOR_WHITE); // Set the text color to white
                    LCD_DisplayStringXY(TFT_GAME_HEADER_TIME_X, TFT_GAME_HEADER_TIME_Y, buf); // Print the time
                }
            }

            bool all_players_dead = true; // Assume all players are dead ;-)

            // For each player do ...
            for(int i = 0; i < PLAYER_COUNT; i++) { 
                player_t* player = &(game->player[i]);  // Copy an object of the current player

                if(game_player_update(game, player, pixels)) { // Update player and execute if, when player state has changed
                    static char buf[15]; // Buffer to hold the text output
                    const char* state_text = "alive"; // Assume that the player is alive
                    
                    if(player->state==dead) { // If the player is dead change the text
                        state_text="dead";
                    }

                    sprintf(buf, "Player%d: %s ", (i+1),state_text); // Format and paste the status to the buffer
                    LCD_SetTextColor(player->color); // Set the text color to the players color
                    LCD_DisplayStringXY(TFT_GAME_HEADER_PLAYER_X+i*TFT_GAME_HEADER_PLAYER_WIDTH, TFT_GAME_HEADER_PLAYER_Y, buf); // Print the status
                }

                if(player->state!=dead) { // If the current player still lives not all players are dead ...
                    all_players_dead=false;
                }
            }

            if(all_players_dead) {  // End the game if all players are dead
                game->state=ended; // Set the state to ended
                return true;
            } else {
                return false;
            }
        }

        case ended:
            while(!io_button_has_edge(BTN_START)); // Wait for the start button to be pressed again
            LCD_Clear(GUI_COLOR_BLACK); // Clear the background
            game->state= prestart; // Set the state to prestart
        return true;
    }
}
