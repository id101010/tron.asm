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
#include "io.h"
#include "game.h"
#include "uart.h"

//Game Object, which contains the entire state of the game
//Exchange this object via Uart, to have remote players
game_t gameobj;

volatile uint64_t ticks = 0; //Ticks since reset
uint64_t lastTicks = 0; //Last tick

//SysTick Handler that will be called several times per second (see TICKS_PER_SECOND). This is an interrupt.
void SysTick_Handler() {
    ticks++; 
    io_process(); //Process button presses
}

#define SYSCLK              168e6 //System frequency
#define TICKS_PER_SECOND    1000 //Number of SysTick interrupts that should occour ( = number of times SysTick_Handler calls per sec)

int main(void)
{
    if(SysTick_Config(SYSCLK/TICKS_PER_SECOND)) { //if systick config fails
        while(1); //sleep forever
    }

    game_init(&gameobj, TICKS_PER_SECOND); //Init game state

    while(1) {
        uint64_t curTicks = ticks;
        if(game_step(&gameobj,curTicks-lastTicks)) { //calculate next game step, and pass it the delta time. Returns true if function was blocking
            lastTicks = ticks; //Save actual ticks
        } else {
            lastTicks = curTicks; //Save ticks from before calling game_step
        }
    }

	return 0;
}

