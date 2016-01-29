#include <lcd.h>
#include <lcd_lld.h>
#include <color.h>
#include <stdio.h>

#include "game.h"
#include "io.h"

void game_init(game_t* game, uint16_t ticks_per_sec) {
    
    // gpio init
    io_init();

    // lcd init
    LCD_Init();
    LCD_Clear(GUI_COLOR_BLACK);
    
    // struct init
    game->state=prestart;
    game->ticks_per_pixel = SPEED_SLOW;
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
            if((int16_t)(player->position.y) - pixels <= TG_FIELD_TOP){
                return true; // Collision at top boundary
            }
            break;
        case down:
            if((int16_t)(player->position.y) + pixels >= (TFT_HEIGHT - TG_FIELD_BOTTOM - 1)){
                return true; // Collision at bottom boundary
            }
            break;
        case left:
            if((int16_t)(player->position.x) - pixels <= TG_FIELD_LEFT){
                return true; // Collision at left boundary
            }
            break;
        case right:
            if((int16_t)(player->position.x) + pixels >= (TFT_WIDTH - TG_FIELD_RIGHT - 1)){
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

    return false;
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

void game_get_color(uint16_t confbits, uint16_t* player1_color, uint16_t* player2_color) {

    //we have 4 bits per player color, we use 1 bit for red, 2 for green, 1 for blue
    //Display color is 16 bit: 5bits red (15-11), 6 green (10-5), 5 blue (4-0)
    *player1_color =0;
    *player2_color =0;
    uint16_t temp;
    if(confbits&0x80) { //player1, red1
        *player1_color|=0xF800; //set full red
    }
    temp = (confbits&0x60) << 1; //move the player1, green1-2 bits to the highest bit position (bits 7-6)
    *player1_color|= temp << 3; //move green into the right position (bits 10-9) 
    if(confbits&0x10) { //player 1, blue1
       *player1_color|=0x1F; //set full blue
    }
    if(*player1_color==0) {
        *player1_color= (3 << 11) | (7 << 5) | (3 << 0); //set color to gray (2 bits active per color, or 3 bits for green)
    }


    if(confbits&0x08) { //player2, red1
        *player2_color|=0xF800; //set full red
    }
    temp = (confbits&0x06) << 5; //move the player2, green1-2 bits to the highest bit position (bits 7-6)
    *player2_color|= temp << 3; //move green into the right position (bits 10-9) 
    if(confbits&0x01) { //player 2, blue1
        *player2_color|=0x1F; //set full blue
    }

    if(*player2_color==0) {
        *player2_color= (3 << 11) | (7 << 5) | (3 << 0); //set color to gray (2 bits active per color, or 3 bits for green)
    }
}

static const char* texts [] = {
    "Config Instructions:",
    "* Change the player colors using the switches S7-S0",
    "* Use the poti to change the game speed",
    "* Press T0 to start the game",
    "",
    "Game Instructions:",
    "* Player 1 Keys: T3 and T2",
    "* Player 2 Keys: T1 and T0",
    "* Stay alive!",
    NULL
};


bool game_step_prestart(game_t* game) {

    //Draw "Player x: Color" Strings
    for(int i=0; i<PLAYER_COUNT; i++) {
        static char buf[16]; // Text buffer
        LCD_SetTextColor(GUI_COLOR_WHITE);
        sprintf(buf, "Player%d: Color", (i+1)); // Print the color and the players name to the text buffer
        LCD_DisplayStringXY(TG_START_X, 
                            TG_START_Y + TG_START_FONT_OFFSET_Y + i* (TG_START_BLOCK_HEIGHT + TG_START_BLOCK_SPACING),
                            buf );
    }
    
    //Draw "Game Speed:" String
    LCD_DisplayStringXY(TG_START_X, 
                        TG_START_Y + PLAYER_COUNT*(TG_START_BLOCK_HEIGHT+TG_START_BLOCK_SPACING) + TG_START_FONT_OFFSET_Y, 
                        "Game Speed:");

    //Draw bounding box of gamespeed bar
    LCD_DrawRect( TG_START_COL2_X, 
                  TG_START_Y + PLAYER_COUNT*(TG_START_BLOCK_HEIGHT+TG_START_BLOCK_SPACING), 
                  TG_START_COL2_WIDTH, 
                  TG_START_BLOCK_HEIGHT,
                  GUI_COLOR_WHITE);

    //Draw Info texts
    int i=0;
    while(texts[i]!=NULL) {
        LCD_DisplayStringXY(TG_START_X, 
                            TG_START_Y + (PLAYER_COUNT + 1)*(TG_START_BLOCK_HEIGHT+TG_START_BLOCK_SPACING) + TG_START_FONT_OFFSET_Y + i * TG_START_FONT_HEIGHT, 
                            texts[i]);
        i++;
    }


    uint8_t switches_old = 0;
    uint16_t adc_old =0;
    uint16_t player1_color=0, player2_color=0,game_speed=0;
    bool first = true; 
    
    while(!io_button_has_edge(BTN_START)) { //As long as nobody presses the start button
        uint8_t switches = read_switches(); //read current switches value
        uint16_t adc = read_adc(); //read current adc value
        if(adc>ADC_MAX) { //adc value exeeds maximum
            adc = ADC_MAX; //set to maximum 
        }

        if(switches!=switches_old || first) { //switch values changed or we are in the first loop
            game_get_color(switches,&player1_color,&player2_color); //calculate colors from switch value
            //Draw player1 color
            LCD_DrawRectF(  TG_START_COL2_X,
                            TG_START_Y,
                            TG_START_COL2_WIDTH,
                            TG_START_BLOCK_HEIGHT,
                            player1_color);
            //Draw player 2 color
            LCD_DrawRectF(  TG_START_COL2_X,
                            TG_START_Y + TG_START_BLOCK_HEIGHT + TG_START_BLOCK_SPACING,
                            TG_START_COL2_WIDTH,
                            TG_START_BLOCK_HEIGHT,
                            player2_color);
            switches_old = switches; //save switch state, to detect edge 
        }

        if(abs( (int16_t) adc - adc_old ) > ADC_TOLERANCE || first) { //adc value changed quite a bit or we are in the first loop
            uint8_t bar_width = (TG_START_COL2_WIDTH -2)*adc/ADC_MAX;
            game_speed =  (ADC_MAX-adc)*(SPEED_SLOW-SPEED_FAST)/ADC_MAX + SPEED_FAST;


            //Draw background part of bar
            LCD_DrawRectF(TG_START_COL2_X + 1 + bar_width, 
                          TG_START_Y + PLAYER_COUNT*(TG_START_BLOCK_HEIGHT+TG_START_BLOCK_SPACING) + 1, 
                          TG_START_COL2_WIDTH - 2 - bar_width,
                          TG_START_BLOCK_HEIGHT-2,
                          GUI_COLOR_BLACK);
            //Draw foreground part of bar
            LCD_DrawRectF( TG_START_COL2_X + 1, 
                          TG_START_Y + PLAYER_COUNT*(TG_START_BLOCK_HEIGHT+TG_START_BLOCK_SPACING) + 1, 
                          bar_width,
                          TG_START_BLOCK_HEIGHT - 2,
                          GUI_COLOR_BLUE);
            
            //For debug purposes    
            /*static char buf[16]; // Text buffer
            LCD_SetTextColor(GUI_COLOR_WHITE);
            sprintf(buf, "speed: %02d", game_speed);
            LCD_DisplayStringXY(TG_START_COL2_X + TG_START_COL2_WIDTH + TG_START_FONT_OFFSET_Y, 
                                TG_START_Y + TG_START_FONT_OFFSET_Y + PLAYER_COUNT*(TG_START_BLOCK_HEIGHT+TG_START_BLOCK_SPACING) + 1, 
                                buf );*/
            
            
            adc_old = adc; //save old adc value, to detect change
        }

        first = false; // we're no longer in the first loop
    }

    // Setup the two players
    //Init player 1
    player_init(&(game->player[0]), // Fill object of player 1
                BTN_PLAYER_1_LEFT,  // Left-button of player1
                BTN_PLAYER_1_RIGHT, // Right-button of player 1
                (point_t) { //Start point of player 1
                    .x=(TG_FIELD_START_OFFSET + TG_FIELD_LEFT), // x start coordinate
                    .y=(((TFT_HEIGHT - TG_FIELD_TOP - TG_FIELD_BOTTOM) / 2) + TG_FIELD_TOP) // y start coordinate
                }, 
                player1_color, // color of player 1
                right); // initial moving direction of player 2

    //Init player 2
    player_init(&(game->player[1]),
                BTN_PLAYER_2_LEFT,
                BTN_PLAYER_2_RIGHT,
                (point_t) {
                    .x=(TFT_WIDTH - 1 - TG_FIELD_RIGHT - TG_FIELD_START_OFFSET), // x start coordinate
                    .y=(((TFT_HEIGHT - TG_FIELD_TOP - TG_FIELD_BOTTOM) / 2) + TG_FIELD_TOP) // y start coordinate
                }, 
                player2_color,
                left);
    
    game->ticks_per_pixel = game_speed;
    game->state = running; // Switch the game state to running
    game->time = 0; // Reset the game time

    LCD_Clear(GUI_COLOR_BLACK); // Clear the background

    // Draw the game boundary
    LCD_DrawRect(TG_FIELD_LEFT, // left top x
                TG_FIELD_TOP, // left top y
                (TFT_WIDTH - TG_FIELD_LEFT - TG_FIELD_RIGHT - 1), // right bottom x
                (TFT_HEIGHT - TG_FIELD_TOP - TG_FIELD_BOTTOM - 1), // right bottom y
                GUI_COLOR_WHITE); // Color of the boundary

    LCD_SetTextColor(GUI_COLOR_WHITE); // Reset color to white
    LCD_DisplayStringXY(TG_HEADER_TIME_X, TG_HEADER_TIME_Y, "Time: 0:00"); // Draw the zero-time

    for(int i = 0; i < PLAYER_COUNT; i++){ // For each player print its name and its state
        static char buf[16]; // Text buffer
        LCD_SetTextColor(game->player[i].color); // Set the text color according to the players color
        sprintf(buf, "Player%d: alive", (i+1)); // Print the state and the players name to the text buffer
        LCD_DisplayStringXY(TG_HEADER_PLAYER_X+i*TG_HEADER_PLAYER_WIDTH, TG_HEADER_PLAYER_Y, buf); // Print everything
    }

    return true;
}

bool game_step_running(game_t* game, uint64_t delta_time)

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
            LCD_DisplayStringXY(TG_HEADER_TIME_X, TG_HEADER_TIME_Y, buf); // Print the time
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
            LCD_DisplayStringXY(TG_HEADER_PLAYER_X+i*TG_HEADER_PLAYER_WIDTH, TG_HEADER_PLAYER_Y, buf); // Print the status
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

bool game_step_ended(game_t* game) {
    while(!io_button_has_edge(BTN_START)); // Wait for the start button to be pressed again
    LCD_Clear(GUI_COLOR_BLACK); // Clear the background
    game->state= prestart; // Set the state to prestart
    return true;
}

bool game_step(game_t* game, uint64_t delta_time) { // Calculate the next game step

    switch(game->state) {
        case prestart: // If the game is in prestart state
            return game_step_prestart(game);
        case running:
            return game_step_running(game,delta_time);
        case ended:
            return game_step_ended(game);
    }
}
