#include <stdbool.h>
#include <stm32f4xx.h>

typedef struct pin_s {
	GPIO_TypeDef* GPIO;
	uint16_t pinmask;
	bool input;
} pin_t;

void pin_create(pin_t* pin, GPIO_TypeDef* GPIO, uint8_t pinnr, bool input);
bool pin_get(pin_t* pin);
void pin_set(pin_t* pin, bool status);
void pin_toggle(pin_t* pin);




