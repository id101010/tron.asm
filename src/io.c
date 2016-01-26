#include "io.h"

//-----------Local types & functions--------------------------
typedef struct pin_s {
	GPIO_TypeDef* GPIO;
	uint16_t pinmask;
	bool input;
} pin_t;

static void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr, bool input);
static bool pin_get(pin_t* pin);
static void pin_set(pin_t* pin, bool status);
static void pin_toggle(pin_t* pin);


//-------------Local Variables-------------------------
static pin_t pin_t0;
static pin_t pin_t1;
static pin_t pin_t2;
static pin_t pin_t3;

static uint8_t new = 0;
static uint8_t old = 0;
static volatile uint8_t edg = 0;

//---------------Implementation --------------------------------
void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr, bool input) {
	GPIO_InitTypeDef gi;
	GPIO_StructInit(&gi);

	gi.GPIO_Pin = 1 << pinnr;

	if(input) {
		gi.GPIO_Mode = GPIO_Mode_IN;
		gi.GPIO_OType = GPIO_OType_OD;
		gi.GPIO_PuPd = GPIO_PuPd_UP;
	} else {
		gi.GPIO_Mode = GPIO_Mode_OUT;
		gi.GPIO_OType = GPIO_OType_PP;
		gi.GPIO_PuPd = GPIO_PuPd_NOPULL;
	}

	GPIO_Init(GPIO,&gi);

	pin->GPIO=GPIO;
	pin->pinmask=0x01<<pinnr;
	pin->input = input;
}


bool pin_get(pin_t* pin) {
	if(pin->input) {
		return GPIO_ReadInputDataBit(pin->GPIO,pin->pinmask);
	} else {
		return GPIO_ReadOutputDataBit(pin->GPIO,pin->pinmask);
	}
}

void pin_set(pin_t* pin, bool status) {
	if(!pin->input) {
		GPIO_WriteBit(pin->GPIO,pin->pinmask,status);
	}
}

void pin_toggle(pin_t* pin) {
	if(!pin->input) {
        pin_set(pin,!pin_get(pin));
	}
}

void io_init(void){
    //gpio init
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE);
    pin_create(&pin_t0, GPIOC, 7, true);
    pin_create(&pin_t1, GPIOB, 15, true);
    pin_create(&pin_t2, GPIOB, 14, true);
    pin_create(&pin_t3, GPIOI, 0, true);

}

void io_process(void) {
	new = pin_get(&pin_t0)     	|
		  pin_get(&pin_t1) << 1	|
		  pin_get(&pin_t2) << 2	|
		  pin_get(&pin_t3) << 3;

	edg |= (new ^ old) & new;    // detect positive edge
	old = new;
}

bool io_button_has_edge(uint8_t btnnumber) {

	uint8_t bm 		= (1 << btnnumber);
	bool status 	= ((edg & bm) > 0);
	
    if(status){
	    edg &= ~bm;
    }

	return status;
}
