#include "Flanger.h"

void apply_flanger(long* value) {
	apply_delay_line(value, map(lfo_value, -128, 127, 5000, 15000)); //Задержим сигнал на время, модулируемое низкочастотным осциллятором
}
