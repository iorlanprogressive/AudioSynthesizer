#include "Oscill.h"

enum effect_value effect = NONE;

float _waves_count = 0.0;
float _detune = 0.0;
float _decrease = 1.0;
unsigned int ampl[FREQ_COUNT] = {0};
unsigned long ampl_threshold = 0;

void waves_count(unsigned long long value) {
	_waves_count = constrain(value, 0.0, 1.0);
}

void detune(float value) {
	_detune = constrain(value, 0.0, 1.0);
}

void decrease(float value) {
	_decrease = 1.0 - constrain(value, 0.0, 1.0);
}

bool oscil_enable(void) {
	return timer_enable(&timer1, 2500, &oscil_handler);
}

bool oscil_disable(void) {
	return timer_disable(timer1);
}

bool oscil_start(void) {
	return timer_start(timer1);
}

bool oscil_stop(void) {
	return timer_stop(timer1);
}

bool oscil_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx) {
	static unsigned long count = 0;
	long value = 0;
	for (unsigned int i = 0; i < FREQ_COUNT; i++) //Пройдёмся по всем частотам
		value += ampl[i] * sin(((long)(count * (float)i / CONFIG_MAX_FREQ) % CONFIG_OSC_RESOLUTION) * OSC_PHASE_COEF); //Получим значение волны определённой частоты с заданной амплитудой
	//Применим выбранный эффект
	if (effect == DELAY)
		apply_delay(&value);
	else if (effect == FLANGER)
		apply_flanger(&value);
	else if (effect == TREMOLO)
		apply_tremolo(&value);//&value);
	//Если используем КЛИППИНГ, применим его
	if (use_clipping)
		apply_clipping(&value);
	apply_volume_adjust(&value); //Значение громкости
	value = map(value, -ampl_threshold, ampl_threshold, -2147483647, 2147483647); //"Растянем" звуковую водну, чтобы громкость не зависела от количества звучащих нот
	write(&value, 4); //Выведем значение на ЦАП
	if (++count == OSC_FREQ) count = 0;
	return true; //Высокоприоритетная задача, т. к. она связана с выводом звука
}
