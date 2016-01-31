//--------Local definitions ---------------------

/*
typedef struct pin_s {
    GPIO_TypeDef* GPIO;
    uint16_t pinmask;
} pin_t;
*/

.set OFFSET_PIN_GPIO, 0
.set OFFSET_PIN_PINMASK, 4

    
//--------Local Functions ------------------ 
.text


//void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr)
.global pin_create

//R0: address to pin struct, R1: address to GPIO, R2: pinnr (1 byte). No return value
pin_create:

    .set OFFSET_MODR, 0
    .set OFFSET_OTYPER, 0x4
    .set OFFSET_PUPDR, 0xC 
    .set GPIO_MODE_IN, 0
    .set GPIO_PuPd_UP, 1
    .set GPIO_OType_OD, 1

    PUSH {r4,r5,r6,lr}

    LSL r4, r2, #1 //r4 = pinnr*2
    MOV r5, #3 //r5 = 3
    LSL r5, r5, r4 //r5 = 3 << (pinnr*2)

    //GPIO->MODER &=~ ( 3 << (pinnr*2)); //Clear the 2 bits (mode)
    LDR r3, [r1, #OFFSET_MODR] //r3 = GPIO->MODR 
    BIC r3, r3, r5 //r3 &= ~r5

    //GPIO->MODER |= GPIO_Mode_IN << (pinnr*2); //Set the correct bits for mode
    MOV r6, #GPIO_MODE_IN
    LSL r6, r6, r4 //r5 = GPIO_MODE_IN << (pinnr*2) 
    ORR r3, r3, r6 //r3 |= r6
    STR r3, [r1, #OFFSET_MODR] //GPIO->MODR = r3

    //GPIO->PUPDR &=~  (3 <<(pinnr*2)); //clear the 2 bits (pp/od)
    LDR r3, [r1, #OFFSET_PUPDR] //r3 = GPIO->PUPDR 
    BIC r3, r3, r5 //r3 &= ~r5

    //GPIO->PUPDR |= GPIO_PuPd_UP <<(pinnr*2); //set the correct bits for pp/od
    MOV r6, #GPIO_PuPd_UP
    LSL r6, r6, r4 //r6 = GPIO_PuPd_UP << (pinnr*2) 
    ORR r3, r3, r6 //r3 |= r6
    STR r3, [r1, #OFFSET_PUPDR] //GPIO->PUPDR = r3
    
    //GPIO->OTYPER &=~ (1 << pinnr); //Clear the bit (otype)
    LDRH r3, [r1, #OFFSET_OTYPER] //r3 = GPIO->OTYPER
    MOV r5, #1 //r5 = 1
    LSL r5, r5, r2 //r5 = 1 << pinnr
    BIC r3, r3, r5 //r3 &= ~r5
    
    //GPIO->OTYPER |= GPIO_OType_OD <<(pinnr*2); //set the correct bit for otype
    MOV r6, #GPIO_OType_OD
    LSL r6, r6, r2 //r6 = GPIO_OType_OD << pinnr 
    ORR r3, r3, r6 //r3 |= r6
    STRH r3, [r1, #OFFSET_OTYPER] //GPIO->OTYPER = r3

    //pin->GPIO=GPIO; // Set the gpiopin in the pin structure
    STR r1, [r0, #OFFSET_PIN_GPIO]

    //pin->pinmask=0x01<<pinnr; // Insert the pinmask
    STRH r5, [r0, #OFFSET_PIN_PINMASK]

    POP {r4, r5, r6, pc}

end_pin_create:


//bool pin_get(pin_t* pin)
.global pin_get

//R0: address to pin struct. Return value in R0 (1 byte)
pin_get:
         
   .set GPIO_IDR, 0x10
    //return ((pin->GPIO->IDR & pin->pinmask) > 0);
   LDR r1, [r0, #OFFSET_PIN_GPIO] //r1 = pin->GPIO
   LDR r1, [r1,#GPIO_IDR] // r1 = pin->GPIO->IDR 
   LDRH r2, [r0, #OFFSET_PIN_PINMASK] // r2 = pin->pinmask
   TST r1, r2 // flags = r1 & r2
   BEQ pin_get_eq //jump to pin_get_eq if (r1 & r2) == 0

   MOV r0, #1 //return 1
   MOV pc, lr
pin_get_eq:
   MOV r0, #0 //return 0
   MOV pc, lr
end_pin_get:


//-------Global Functions-------------------
//void adc_init(void);
.global adc_init

//No Parameters, No Return value
adc_init:
  
    .set GPIO_MODER_MODER0, 3
    .set GPIO_PUPDR_PUPDR0, 3
    .set GPIO_Mode_AN, 3
    .set RCC_APB2Periph_ADC, 0x100
    .set ADC_Resolution_10b, 0x01000000
    .set ADC_CR2_ADON, 1
    .set OFFSET_CR1, 0x4
    .set OFFSET_SQR1, 0x2c
    .set OFFSET_CR2, 0x8

    LDR r1, =0x40020400 //r1 = GPIOB

    //GPIOB->MODER &= ~GPIO_MODER_MODER0;
    LDR r3, [r1, #OFFSET_MODR] //r3 = GPIOB->MODR 
    MOV r2, #GPIO_MODER_MODER0 
    BIC r3, r3, r2 //r3 &= ~GPIO_MODER_MODER0

    //GPIOB->MODER |=  GPIO_Mode_AN;
    MOV r2, #GPIO_Mode_AN 
    ORR r3, r3, r2 //r3 |= GPIO_Mode_AN
    STR r3, [r1, #OFFSET_MODR] //GPIOB->MODR = r3

    //GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0;
    LDR r3, [r1, #OFFSET_PUPDR] //r3 = GPIOB->PUPDR 
    MOV r2, #GPIO_PUPDR_PUPDR0
    BIC r3, r3, r2 //r3 &= ~GPIO_PUPDR_PUPDR0

    //GPIOB->PUPDR |=  GPIO_PuPd_UP;
    MOV r2, #GPIO_PuPd_UP
    ORR r3, r3, r2 //r3 |= GPIO_PuPd_UP 
    STR r3, [r1, #OFFSET_PUPDR] //GPIOB->PUPDR = r3


    //RCC->APB2ENR |= RCC_APB2Periph_ADC;
    LDR r1, =0x40023844 //RCC base: 0x40023800, APB2ENR offset: 0x44
    LDR r3, [r1] //r3 = RCC->APB2ENR 
    LDR r2, =#RCC_APB2Periph_ADC 
    ORR r3, r3, r2 //r3 |= RCC_APB2Periph_ADC
    STR r3, [r1]
      
    //ADC1->CR1 = ADC_Resolution_10b;
    LDR r1, =0x40012000 //Adc base adress 
    LDR r2, =#ADC_Resolution_10b
    STR r2, [r1, #OFFSET_CR1]
    
    //ADC1->SQR1 = 0;
    MOV r2, #0
    STR r2, [r1, #OFFSET_SQR1]

    //ADC1->CR2 = ADC_CR2_ADON;
    MOV r2, #ADC_CR2_ADON
    STR r2, [r1, #OFFSET_CR2]

    MOV pc, lr

end_adc_init:


//uint16_t read_adc()
.global read_adc

//No Parameters, Return value in R0 (halfword, 2bytes)
read_adc:
    .set OFFSET_SMPR2, 0x10
    .set OFFSET_SQR3, 0x34
    .set OFFSET_SR, 0
    .set OFFSET_DR, 0x4C
    .set ADC_Channel_8, 8 
    .set SMPR1_SMP_SET, 0x07
    .set ADC_SampleTime_15Cycles, 1
    .set SQR3_SQ_SET, 0x1F
    .set ADC_CR2_SWSTART, 0x40000000
    .set ADC_FLAG_EOC, 0x2
    .set ADC_MASK, 0x3FF

    
    LDR r1, =0x40012000; //ADC base address
    MOV r0, #3*ADC_Channel_8 //r0 = 3 * ADC_Channel_8

    //ADC1->SMPR2 &= SMPR1_SMP_SET           << (3 * ADC_Channel_8);
    LDR r3, [r1, #OFFSET_SMPR2] //r3 = ADC1->SMPR2 
    MOV r2, #SMPR1_SMP_SET
    LSL r2, r2, r0 // r2 = SMPR1_SMP_SET  << (3 * ADC_Channel_8)
    BIC r3, r3, r2 // r3 &= ~r2

    //ADC1->SMPR2 |= ADC_SampleTime_15Cycles << (3 * ADC_Channel_8);
    MOV r2, #ADC_SampleTime_15Cycles
    LSL r2, r2, r0 // r2 = ADC_SampleTime_15Cycles << (3 * ADC_Channel_8)
    ORR r3, r3, r2 // r3 |= r2
    STR r3, [r1, #OFFSET_SMPR2] // ADC1->SMPR2 = r3 

    //ADC1->SQR3  &= ~SQR3_SQ_SET;
    LDR r3, [r1, #OFFSET_SQR3] // r3 = ADC1->SQR3
    MOV r2, #SQR3_SQ_SET
    BIC r3, r3, r2 // r3 &= ~r2
    
    //ADC1->SQR3  |= ADC_Channel_8;
    MOV r2, #ADC_Channel_8
    ORR r3, r3, r2 // r3 |= r2
    STR r3, [r1, #OFFSET_SQR3] // ADC1->SQR3 = r3

    //ADC1->CR2 |= ADC_CR2_SWSTART;   
    LDR r3, [r1, #OFFSET_CR2] // r3 = ADC1->CR2
    LDR r2, =#ADC_CR2_SWSTART
    ORR r3, r3, r2 // r3 |= r2
    STR r3, [r1, #OFFSET_CR2] // ADC1->CR2 = r3

    //while( (ADC1->SR & ADC_FLAG_EOC) == 0 );
read_adc_loop:
    LDR r3, [r1, #OFFSET_SR] //r3 = ADC1->SR
    MOV r2, #ADC_FLAG_EOC
    TST r3, r2 //flags = r3 & r2
    BEQ read_adc_loop //jump back to loop beginning while (r3&r2) == 0

    //uint16_t value = 0;
    //value = ADC1->DR;
    MOV r2, #OFFSET_DR
    LDRH r0, [r1, r2] //r0 = ADC1->DR

    //value &= ADC_MASK;
    LDR r2, =#ADC_MASK
    AND r0, r0, r2 // r0 &= r2
    
    //return value;
    MOV pc, lr

end_read_adc:



//uint8_t read_switches()
.global read_switches

//No Parameters, Return value in R0 (1 byte)
read_switches:

    .set ADDR_LEDS, 0x6C000200
    .set ADDR_SWITCHES, 0x6C000400

    LDR r1, =ADDR_SWITCHES  // load switch address to r1
    LDR r0, [r1]            // load switch value to r0
    LDR r2, =ADDR_LEDS      // load led address to r1
    STR r0, [r2]            // store switch value at led address

    MOV pc, lr

end_read_switches:
