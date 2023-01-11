#include "Delay.h"

unsigned long _delay = CONFIG_MAX_DELAY;

void delay(unsigned long value) {
	_delay = constrain(value, 0, CONFIG_MAX_DELAY);
}

void apply_delay(long* value) {
	apply_delay_line(value, _delay);
}
