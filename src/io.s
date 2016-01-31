//---------Local variables ----------------------
.data



/*
typedef struct pin_s {
    GPIO_TypeDef* GPIO;
    uint16_t pinmask;
} pin_t;
*/

.set OFFSET_PIN_GPIO, 0
.set OFFSET_PIN_PINMASK, 8


/*
static pin_t pin_t0;
static pin_t pin_t1;
static pin_t pin_t2;
static pin_t pin_t3;
*/


pin0: .space 8, 0 //storage for GPIO ptr
.hword 0 //storage for pinmask
pin1: .space 8, 0
.hword 0
pin2: .space 8, 0
.hword 0
pin3: .space 8, 0
.hword 0



    
//--------Local Functions ------------------ 
.text


//void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr)

//R0: address to pin struct, R1: address to GPIO, R2: pinnr (1 byte). No return value
pin_create:

    .set OFFSET_MODR, 0
    .set OFFSET_OTYPER, 0x4
    .set OFFSET_PUPDR, 0xC 
    .set GPIO_MODE_IN, 0
    .set GPIO_PuPd_UP, 1
    .set GPIO_OType_OD, 1

    LSL r4, r2, #1 //r4 = pinnr*2
    MOV r5, #3 //r5 = 3
    LSL r5, r5, r4 //r5 = 3 << (pinnr*2)

    //GPIO->MODER &=~ ( 3 << (pinnr*2)); //Clear the 2 bits (mode)
    LDRB r3, [r1, #OFFSET_MODR] //r3 = GPIO->MODR 
    BIC r3, r3, r5 //r3 &= ~r5

    //GPIO->MODER |= GPIO_Mode_IN << (pinnr*2); //Set the correct bits for mode
    MOV r6, #GPIO_MODE_IN
    LSL r6, r6, r4 //r5 = GPIO_MODE_IN << (pinnr*2) 
    ORR r3, r3, r6 //r3 |= r6
    STRB r3, [r1, #OFFSET_MODR] //GPIO->MODR = r3

    //GPIO->PUPDR &=~  (3 <<(pinnr*2)); //clear the 2 bits (pp/od)
    LDRB r3, [r1, #OFFSET_PUPDR] //r3 = GPIO->PUPDR 
    BIC r3, r3, r5 //r3 &= ~r5

    //GPIO->PUPDR |= GPIO_PuPd_UP <<(pinnr*2); //set the correct bits for pp/od
    MOV r6, #GPIO_PuPd_UP
    LSL r6, r6, r4 //r6 = GPIO_PuPd_UP << (pinnr*2) 
    ORR r3, r3, r6 //r3 |= r6
    STRB r3, [r1, #OFFSET_PUPDR] //GPIO->PUPDR = r3
    
    //GPIO->OTYPER &=~ (1 << pinnr); //Clear the bit (otype)
    LDRB r3, [r1, #OFFSET_OTYPER] //r3 = GPIO->OTYPER
    MOV r5, #1 //r5 = 1
    LSL r5, r5, r2 //r5 = 1 << pinnr
    BIC r3, r3, r5 //r3 &= ~r5
    
    //GPIO->OTYPER |= GPIO_OType_OD <<(pinnr*2); //set the correct bit for otype
    MOV r6, #GPIO_OType_OD
    LSL r6, r6, r2 //r6 = GPIO_OType_OD << pinnr 
    ORR r3, r3, r6 //r3 |= r6
    STRB r3, [r1, #OFFSET_OTYPER] //GPIO->OTYPER = r3

    //pin->GPIO=GPIO; // Set the gpiopin in the pin structure
    STR r1, [r0, #OFFSET_PIN_GPIO]

    //pin->pinmask=0x01<<pinnr; // Insert the pinmask
    STR r5, [r0, #OFFSET_PIN_PINMASK]


    //TODO: Implement

end_pin_create:


//bool pin_get(pin_t* pin)

//R0: address to pin struct. Return value in R0 (1 byte)
pin_get:

    //TODO: Implement

end_pin_get:


//-------Global Functions-------------------
//void io_init(void);
.global io_init

//No Parameters, No Return value
io_init:

    //convention for idermiediate labels. io_init_<whatever>
    //TODO: Implement


    //BL pin_create (4x)


end_io_init:



//void io_process(void)
.global io_process

//No Parameters, No Return value
io_process:

    //TODO: Implement

end_io_process:



//bool io_button_has_edge(uint8_t btnnumber)
.global io_button_has_edge

//R0: pin nr (1 byte), Return value in R0 (1 byte)
io_button_has_edge:

    //TODO: Implement

end_io_button_has_edge:



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




