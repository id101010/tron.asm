#include <color.h>

#include "player.h"

void player_init(player_t* player, uint8_t btn_left, uint8_t btn_right, 
                 point_t pos, uint16_t color, direction_t direction) {
    player->color = color;
    player->num_positions = 1;
    player->position = pos;
    player->past_positions[0] = player->position;
    player->direction = direction;
    player->state = alive;
    player->btn_left = btn_left;
    player->btn_right = btn_right;
}


void player_append_position(player_t* player, point_t point) {
    if(player->num_positions < max_positions) {
        player->past_positions[player->num_positions++] = point;
    } 
} 
