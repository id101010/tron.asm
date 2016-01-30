#ifndef GAME_H
#define GAME_H

#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include"player.h"


//---- Section 1 - General ------------------------------------------------------------------------------
//---- Section 1.1  - Player defintions -----------------------------------------------------------------
#define PLAYER_COUNT    2   //Number of players in the game (incrementing this, will probably require some changes in game.c as well)

//---- Section 1.2 - Speed definitions ------------------------------------------------------------------
#define SPEED_SLOW      75  //Slowest game speed (ticks per pixel)
#define SPEED_FAST      5   //Fastest game speed (ticks per pixel)

//---- Section 1.3 - Button definitions -----------------------------------------------------------------
// Each of the following defines must be number between 0-3 (for T0...T3 on Carme)
#define BTN_START               0  //Button to start the game, restart, continue
// Buttons to control the players
#define BTN_PLAYER_1_LEFT       3
#define BTN_PLAYER_1_RIGHT      2
#define BTN_PLAYER_2_LEFT       1
#define BTN_PLAYER_2_RIGHT      0


//---- Section 2 - Display & Pixel Geometry -------------------------------------------------------------
//---- Section 2.1  - During Active Game ----------------------------------------------------------------

//Margins of the Game field to the display border
#define TG_FIELD_TOP                  20
#define TG_FIELD_BOTTOM               5
#define TG_FIELD_LEFT                 5
#define TG_FIELD_RIGHT                5

#define TG_FIELD_START_OFFSET         10    //Offset from the game-border to the start position of the players

#define TG_HEADER_TIME_X              5     //x position of the Time-Text in the game-header
#define TG_HEADER_TIME_Y              3     //y position of the Time-Text in the game-header
#define TG_HEADER_PLAYER_X            100   //x position of the Player1 state-text in the game-header
#define TG_HEADER_PLAYER_Y            3     //y position of the Player state-texts in the game-header
#define TG_HEADER_PLAYER_WIDTH        100   //width of one Player state-text

//---- Section 2.2  - During an ended game (kill screen) ------------------------------------------------
//Margins of the kill screen to the display border
#define TG_END_TOP                    180
#define TG_END_BOTTOM                 10
#define TG_END_LEFT                   30
#define TG_END_RIGHT                  30

#define TG_END_FONT_OFFSET            5     //Start offset (x,y) from the font to the topleft corner of the kill screen
#define TG_END_FONT_HEIGHT            20    //Height of the font (kill screen)


//---- Section 2.3  - Before the game start (config screen) ---------------------------------------------
#define TG_START_X                    10    //x position of the content
#define TG_START_Y                    10    //y position of the content
#define TG_START_BLOCK_HEIGHT         20    //height of one line of content (aka block)
#define TG_START_BLOCK_SPACING        5     //space between one line of content
#define TG_START_FONT_OFFSET_Y        5     //y offset for the font, to the start of the block
#define TG_START_FONT_HEIGHT          10    //height of the used font
#define TG_START_COL2_X               90    //the x position of the second content column
#define TG_START_COL2_WIDTH           100   //the width of the second content column

//----------------------------------------------------------------------------------------------------

/**
 * @brief Game data type which contains all game data and players.
 *
 */
typedef struct game_s{
    //public section ahead
    
    uint16_t time;              // Seconds since game start
    uint16_t ticks_per_sec;     // Number of game ticks per second
    int8_t winner_id;           // Player who won the previous round
    uint8_t ticks_per_pixel;    // Ticks that are needed to move the player by one pixel
    player_t player[PLAYER_COUNT];

    enum{               // Current state of the game
        init,
        prestart,
        running,
        ended
    } state;

    //private section ahead
    uint16_t ticks_leftover;     // Ticks which were not used in the last round, to advance the pixels
    uint16_t ticks_sum_sec;      // Ticks which were not used in the last round, to advance the seconds
} game_t;

/**
    @brief Initializes the game object
    @param game             Game object
    @param ticks_per_sec    Number of game ticks per second
*/
void game_init(game_t* game, uint16_t ticks_per_sec);

/**
 *   @brief Calculates one step of the game
 *
 *   @param game        Game to calculate a step for
 *   @param deltaTime   Time that passed since the last call to this method (in ticks)
 *   @return            true if the next call to this method should be made with a delta time of zero.
 */
bool game_step(game_t* game, uint64_t deltaTime);

#endif /* GAME_H */
