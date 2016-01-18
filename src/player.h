#ifndef PLAYER_H
#define PLAYER_H


#include<stdlib.h>
#include<stdint.h>

#define max_positions 320

typedef struct point_s{
    uint16_t x;
    uint8_t y;
} point_t;

typedef struct player_s {

    uint8_t id;
    uint16_t color;
    uint16_t num_positions;
    
    enum {
        right, 
        down,
        left, 
        up
    } direction;

    enum{
        dead,
        alive,
    } state;

    point_t past_positions[max_positions];
    point_t position;

} player_t;

void player_init(player_t* player);                                 // reset all fields
void player_append_position(player_t* player, point_t point);       // updates num_position and adds current position to the list

#endif /* PLAYER_H */
