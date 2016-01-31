
    
//--------Local Functions ------------------ 
//void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr)

//R0: address to pin struct, R1: address to GPIO, R2: pinnr (1 byte). No return value
pin_create:

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




