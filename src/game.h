#ifndef GAME_H
#define GAME_H

#include<stdint.h>
#include<stdlib.h>
#include"player.h"

#define PLAYER_COUNT 2
#define PLAYER_WIDTH 3
#define SPEED_SLOW 10

typedef struct game_s{
    //public section
    
    uint16_t time;          // seconds since game start
    int8_t winner_id;
    uint8_t host_id;
    uint8_t ticks_per_pixel;
    player_t player[PLAYER_COUNT];

    enum{
        prestart,
        running,
        ended
    } state;


    //private section ahead:
    uint8_t ticks_leftover;
} game_t;

void game_init(game_t* game);
void game_step(game_t* game, uint64_t deltaTime);

#endif /* GAME_H */
