#include <stm32f4xx.h>
#include <carme_io2.h>
#include "io.h"

//Structure which holds the configuration for one (input) pin (GPIO).
typedef struct pin_s {
    GPIO_TypeDef* GPIO; //GPIO port which is used
    uint16_t pinmask; //Pin Nr (0..15)
} pin_t;

// Local variables
// Pin structs for the pins T0..T3
static pin_t pin_t0;
static pin_t pin_t1;
static pin_t pin_t2;
static pin_t pin_t3;

//Variables for the edge detection
static uint8_t old = 0; //old state of the buttons
static volatile uint8_t edg = 0; //detected edges

//Function prototypes of functions that are implemented in asm
void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr); //Initializes the pin structure and the gpio with the passed data.
bool pin_get(pin_t* pin); //Returns the current state of the pin (true = pin is "high")
bool adc_init(); //Initializes the adc and configures the channel

void io_process(void) {
    //Save the current state of the 4 pins in the lower nibble
    uint8_t new = pin_get(&pin_t0) |
            pin_get(&pin_t1) << 1  |
            pin_get(&pin_t2) << 2  |
            pin_get(&pin_t3) << 3;

    edg |= (new ^ old) & new;    // detect positive edge
    old = new; //store current state, for later edge detection
}

bool io_button_has_edge(uint8_t btnnumber) {
    uint8_t bm      = (1 << btnnumber); // create bitmask
    bool status     = ((edg & bm) > 0); // check if button is pressed
    
    if(status) { // if button is pressed
        edg &= ~bm; // clear edge bit
    }

    return status;
}

void io_init(void){
    //Enable clock for gpio
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE); // Enable gpio clock source
    RCC->AHB1ENR|= RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI;

    // Create player pins
    pin_create(&pin_t0, GPIOC, 7);  // create pin_t0
    pin_create(&pin_t1, GPIOB, 15); // create pin_t1
    pin_create(&pin_t2, GPIOB, 14); // create pin_t2
    pin_create(&pin_t3, GPIOI, 0);  // create pin_t3

    adc_init(); //Initialize adc

}
