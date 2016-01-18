/*****************************************************************************
 * Copyright &copy; 2013, Bern University of Applied Sciences.
 * All rights reserved.
 *
 * ##### GNU GENERAL PUBLIC LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 *****************************************************************************/
#include <stm32f4xx.h>				// Processor STM32F407IG
#include <carme.h>					// CARME Module
#include <lcd.h>
#include <lcd_lld.h>
#include <color.h>
#include <stm32f4xx_adc.h>
#include "player.h"
#include "game.h"
#include "uart.h"


game_t gameobj;

uint64_t ticks;
uint64_t lastTicks;

void SysTick_Handler() {
    ticks++;
}


#define SYSCLK 168e6
#define TICKS_PER_SECOND 100

int main(void)
{
    if(SysTick_Config(SYSCLK/TICKS_PER_SECOND)) { //if systick config fails
        while(1); //sleep forever
    }

    game_init(&gameobj);
    while(1) {
        uint64_t curTicks = ticks;
        game_step(&gameobj,curTicks-lastTicks); //calculate next game step, and pass it the delta time
        lastTicks = curTicks;
    }

	return 0;
}

