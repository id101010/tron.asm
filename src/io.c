#include "io.h"

// Local functions
typedef struct pin_s {
    GPIO_TypeDef* GPIO;
    uint16_t pinmask;
    bool input;
} pin_t;

static void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr, bool input);
static bool pin_get(pin_t* pin);
static void pin_set(pin_t* pin, bool status);
static void pin_toggle(pin_t* pin);

// Local variables
static pin_t pin_t0;
static pin_t pin_t1;
static pin_t pin_t2;
static pin_t pin_t3;
static uint8_t new = 0;
static uint8_t old = 0;
static volatile uint8_t edg = 0;

void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr, bool input) {
    GPIO_InitTypeDef gi;  // Create gpio init structure
    GPIO_StructInit(&gi); // Fill gpio init structure with defaults

    gi.GPIO_Pin = 1 << pinnr; // create bitmask out of pin number

    if(input) { // If the pin is set to be an input
        gi.GPIO_Mode    = GPIO_Mode_IN;     // Set mode to input
        gi.GPIO_OType   = GPIO_OType_OD;    // Set type to open drain
        gi.GPIO_PuPd    = GPIO_PuPd_UP;     // Set a pullup
    } else {
        gi.GPIO_Mode    = GPIO_Mode_OUT;    // Set mode to output
        gi.GPIO_OType   = GPIO_OType_PP;    // Set type to pushpull
        gi.GPIO_PuPd    = GPIO_PuPd_NOPULL; // Set no pullup
    }

    GPIO_Init(GPIO,&gi); // Update the GPIO configuration

    pin->GPIO=GPIO; // Set the gpiopin in the pin structure
    pin->pinmask=0x01<<pinnr; // Insert the pinmask
    pin->input = input; // Store the input information
}

bool pin_get(pin_t* pin) {
    if(pin->input) { // If the pin is an input
        return GPIO_ReadInputDataBit(pin->GPIO,pin->pinmask); // Read its value
    } else { // If the pin is an output
        return GPIO_ReadOutputDataBit(pin->GPIO,pin->pinmask); // Read its set value
    }
}

void pin_set(pin_t* pin, bool status) {
    if(!pin->input) { // If the pin isn't an input
        GPIO_WriteBit(pin->GPIO,pin->pinmask,status); // Set its value accordingly
    }
}

void pin_toggle(pin_t* pin) {
    if(!pin->input) { // If the pin isn't an input
        pin_set(pin,!pin_get(pin)); // Toggle its value
    }
}

void io_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE); // Enable gpio clock source
    pin_create(&pin_t0, GPIOC, 7, true);  // create pin_t0
    pin_create(&pin_t1, GPIOB, 15, true); // create pin_t1
    pin_create(&pin_t2, GPIOB, 14, true); // create pin_t2
    pin_create(&pin_t3, GPIOI, 0, true);  // create pin_t3

}

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
