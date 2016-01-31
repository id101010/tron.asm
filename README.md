# TRON.asm

## Description

TRON.asm is a tron clone written for the bfh carme kit, which is based on a stm32 controller.
It is a project for the firmware development class. All I/O parts were developped in assembly language using the thumb instruction set.
The rest of it is written in the programming language c.

## Project Structure

img/*               - GIMP Project for the welcome screen
libs/*              - STM32f4xx libraries
utils/*             - Useful scripts 

src/bitmap.h        - Contains a welcome screen
src/draw.{c.h}      - Draw a bitmap
src/game.{c,h}      - Main game logic
src/main.c          - Systick handler and programm start
src/io.{s,h}        - IO functions implemented in assembly language
src/player.{c,h}    - Implementation of the player object
src/startup.s       - Startup code
src/syscalls.c      - Syscall lowlevel functions (Atollic true studio)
src/io.c.unused     - Old io-fuctions implemented in c programming language.

## How to Build and flash

- Clone the repository
- Build the software by using our Makefile
- Type "make all flash" to build the software and flash it to the board. Be aware that we are using a rather large bitmap, so flashing will take some time.
- Type "make all debug" to build the software and start the gdb debugger.
