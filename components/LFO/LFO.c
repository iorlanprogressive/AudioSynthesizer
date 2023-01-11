#include "LFO.h"

unsigned long _period = 1000000;
char _ampl = 127;
char lfo_value = 0;

enum wave_type wave = SIN;

void rate(float value) {
	_period = 1000000 / constrain(value, 1.0, 3.0); //Установим период
}

void depth(char value) {
	_ampl = constrain(value, 0, 127);
	lfo_value = constrain(lfo_value, -_ampl, _ampl); //После изменения амплитуды текущее значение не должно выходить за пределы
}

bool lfo_enable(void) {
	return timer_enable(&timer3, 1000000, &lfo_handler); //Запустим прерывания по таймеру
}

bool lfo_disable(void) {
	return timer_disable(timer3);
}

bool lfo_start(void) {
	return timer_start(timer3);
}

bool lfo_stop(void) {
	return timer_stop(timer3);
}

bool lfo_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx) {
	static uint32_t count = 0;
	static bool _direct = true;
	switch (wave) {
	case SQ: //МЕАНДР
		if (count * 10 >= _period / 2) { //Если прошёл полупериод, поменяем значение на противоположное (знак амплитуды)
			if (lfo_value >= 0)
				lfo_value = -_ampl;
			else
				lfo_value = _ampl;
			count = 0;
		}
		break;
	case TRN: //ТРЕУГОЛЬНИК
		if (count >= _period / CONFIG_LFO_RESOLUTION) { //Пора сделать очередной "шаг"
			if (_direct) { //Если идём вверх
				if (lfo_value < _ampl) //Если ещё не дошли
					lfo_value++; //Инкрементируем значение
				else //Достигли максимума (амплитуды)
					_direct = false; //Пойдём вниз
			} else { //Если идём вниз
				if (lfo_value > -_ampl) //Если ещё не дошли
					lfo_value--; //Декрементируем значение
				else //Достигли минимума (амплитуды)
					_direct = true; //Gjql`v ddth[
			}
			count = 0;
		}
		break;
	case SAW: //ПИЛА
		if (count >= _period / CONFIG_LFO_RESOLUTION) { //Пора сделать очередной "шаг"
			if (lfo_value < _ampl) //Если ещё не дошли до пика
				lfo_value++; //Инкрементируем значение
			else //Если на пике
				lfo_value = -_ampl; //Сбросим значение (0 - середина, поэтому амплитуда с противоположным знаком)
			count = 0;
		}
		break;
	default: //СИНУСОИДА
		if (count % (_period / CONFIG_LFO_RESOLUTION) == 0) { //Пора сделать очередной "шаг"
			lfo_value = _ampl * sin(((unsigned long)(count / (float)_period / CONFIG_LFO_RESOLUTION) % CONFIG_LFO_RESOLUTION) * LFO_PHASE_COEF); //Синус даёт значения в диапазоне [-1, 1], поэтому умножим на желаемую амплитуду
		}
		break;
	}
	count++;
	return false; //Низкоприоритетная задача
}
