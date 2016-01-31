
    
//--------Local Functions ------------------ 
//void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr)

//R0: address to pin struct, R1: address to GPIO, R2: pinnr (1 byte). No return value
pin_create:

    .set OFFSET_MODR, 0
    .set OFFSET_OTYPER, 0x4
    .set OFFSET_PUPDR, 0xC 
    .set GPIO_MODE_IN, 0

    MOV r5, #3 //r5 = 3
    LSL r4, r2, #1 //r4 = pinnr*2
    LSL r5, r5, r4 //r5 = 3 << (pinnr*2)

    //GPIO->MODER &=~ ( 3 << (pinnr*2)); //Clear the 2 bits (mode)
    LDR r3, [r1, #OFFSET_MODR] //r3 = GPIO->MODR 
    BIC r3, r3, r5 //r3 &= ~r5

    //GPIO->MODER |= GPIO_Mode_IN << (pinnr*2); //Set the correct bits for mode
    MOV r5, #GPIO_MODE_IN
    LSL r5, r5, r4 //r5 = GPIO_MODE_IN << (pinnr*2) 
    ORR r3, r3, r5 //r3 |= r5
    STR r3, [r1, #OFFSET_MODR] //GPIO->MODR = r3

    //GPIO->OTYPER &=~ (1 << (pinnr*2)); //Clear the bit (otype)
    
    
    //GPIO->OTYPER |= GPIO_OType_OD <<(pinnr*2); //set the correct bit for otype
/*    
    GPIO->PUPDR &=~  (3 <<(pinnr*2)); //clear the 2 bits (pp/od)
    GPIO->PUPDR |= GPIO_PuPd_UP <<(pinnr*2); //set the correct bits for pp/od

    pin->GPIO=GPIO; // Set the gpiopin in the pin structure
    pin->pinmask=0x01<<pinnr; // Insert the pinmask
*/



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




