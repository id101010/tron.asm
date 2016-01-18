#include "io.h"


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
		set_pin(pin,!get_pin(pin));
	}
}



