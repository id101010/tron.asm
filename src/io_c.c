#include <stm32f4xx.h>
#include <carme_io2.h>
#include "io.h"


typedef struct pin_s {
    GPIO_TypeDef* GPIO;
    uint16_t pinmask;
} pin_t;

// Local variables
static pin_t pin_t0;
static pin_t pin_t1;
static pin_t pin_t2;
static pin_t pin_t3;
static uint8_t new = 0;
static uint8_t old = 0;
static volatile uint8_t edg = 0;

//Function prototypes of functions that are implemented in asm
void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr);
bool pin_get(pin_t* pin);
bool adc_init();


void io_process(void) {
    new = pin_get(&pin_t0)      |
          pin_get(&pin_t1) << 1 |
          pin_get(&pin_t2) << 2 |
          pin_get(&pin_t3) << 3;

    edg |= (new ^ old) & new;    // detect positive edge
    old = new;
}

bool io_button_has_edge(uint8_t btnnumber) {
    uint8_t bm      = (1 << btnnumber); // create bitmask
    bool status     = ((edg & bm) > 0); // check if button is pressed
    
    if(status){
        edg &= ~bm; // clear edge bit
    }

    return status;
}

void io_init(void){
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE); // Enable gpio clock source
    RCC->AHB1ENR|= RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI;
    // Create player pins
    pin_create(&pin_t0, GPIOC, 7);  // create pin_t0
    pin_create(&pin_t1, GPIOB, 15); // create pin_t1
    pin_create(&pin_t2, GPIOB, 14); // create pin_t2
    pin_create(&pin_t3, GPIOI, 0);  // create pin_t3

    adc_init();

}
