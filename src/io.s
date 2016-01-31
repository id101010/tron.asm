//---------Local variables ----------------------
.data



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

    //TODO: Implement

end_adc_init:


//uint16_t read_adc()
.global read_adc

//No Parameters, Return value in R0 (halfword, 2bytes)
read_adc:

    //TODO: Implement

end_read_adc:



//uint8_t read_switches()
.global read_switches

//No Parameters, Return value in R0 (1 byte)
read_switches:

    //TODO: Implement

end_read_switches:




