#include <stm32f4xx.h>
#include <carme_io2.h>
#include "io.h"

// Local functions
typedef struct pin_s {
    GPIO_TypeDef* GPIO;
    uint16_t pinmask;
} pin_t;

static void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr);
static bool pin_get(pin_t* pin);

// Local variables
static pin_t pin_t0;
static pin_t pin_t1;
static pin_t pin_t2;
static pin_t pin_t3;
static uint8_t new = 0;
static uint8_t old = 0;
static volatile uint8_t edg = 0;

//Memory mapped IO's
static volatile unsigned char* LED = (volatile unsigned char*)0x6C000200;
static volatile unsigned char* SWITCH = (volatile unsigned char*)0x6C000400;

#define SQR3_SQ_SET               ((uint32_t)0x0000001F)  
#define SMPR1_SMP_SET             ((uint32_t)0x00000007)  

void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr) {
    GPIO_InitTypeDef gi;  // Create gpio init structure
    GPIO_StructInit(&gi); // Fill gpio init structure with defaults

    gi.GPIO_Pin = 1 << pinnr; // create bitmask out of pin number

    gi.GPIO_Mode    = GPIO_Mode_IN;     // Set mode to input
    gi.GPIO_OType   = GPIO_OType_OD;    // Set type to open drain
    gi.GPIO_PuPd    = GPIO_PuPd_UP;     // Set a pullup

    GPIO_Init(GPIO,&gi); // Update the GPIO configuration

    pin->GPIO=GPIO; // Set the gpiopin in the pin structure
    pin->pinmask=0x01<<pinnr; // Insert the pinmask
}

bool pin_get(pin_t* pin) {
     return GPIO_ReadInputDataBit(pin->GPIO,pin->pinmask); // Read its value
}

void io_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE); // Enable gpio clock source
    // Create player pins
    pin_create(&pin_t0, GPIOC, 7);  // create pin_t0
    pin_create(&pin_t1, GPIOB, 15); // create pin_t1
    pin_create(&pin_t2, GPIOB, 14); // create pin_t2
    pin_create(&pin_t3, GPIOI, 0);  // create pin_t3
    
    //Enable the peripheral clock of GPIOB
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB;
    //Choose the working mode of PB0 with the GPIO port mode register
    GPIOB->MODER &= ~GPIO_MODER_MODER0;
    GPIOB->MODER |=  GPIO_Mode_AN;
    //Configure the GPIO port pull-up/pull-down register for PB0
    //Page 282/1718 of "RM0090 Reference Reference Manual (October 2014)"
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0;
    GPIOB->PUPDR |=  GPIO_PuPd_UP;
    //Initialize the ADC
    //Enable the peripheral clock of the ADC
    RCC->APB2ENR |= RCC_APB2Periph_ADC;
    //Configure ADC1: scan conversion mode and resolution
    //Set SCAN bit according to ADC_ScanConvMode value
    //Set RES bit according to ADC_Resolution value
    ADC1->CR1 = ADC_Resolution_10b;
    // Configure ADC1: regular channel sequence length
    // Set L bits according to ADC_NbrOfConversion value
    ADC1->SQR1 = 0;
    // Set the ADON bit to enable the ADC
    ADC1->CR2 = ADC_CR2_ADON;
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

uint16_t read_adc(){
    uint16_t value = 0;
    
    // Specify the sample time for the conversion
    ADC1->SMPR2 &= SMPR1_SMP_SET           << (3 * ADC_Channel_8);
    ADC1->SMPR2 |= ADC_SampleTime_15Cycles << (3 * ADC_Channel_8);
    // Set the channel 8 as the first conversion in the ADC reg seq register
    ADC1->SQR3  &= ~SQR3_SQ_SET;
    ADC1->SQR3  |= ADC_Channel_8;
    // Start the conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;   
    // Wait until the conversion has been done
    while( (ADC1->SR & ADC_FLAG_EOC) == 0 );
    // Read the value
    value = ADC1->DR;
    value &= ADC_MASK;
    
    return value;
}

uint8_t read_switches() {
    *LED=*SWITCH;
    return *SWITCH;
}
