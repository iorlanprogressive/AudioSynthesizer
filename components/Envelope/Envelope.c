#include "Envelope.h"

unsigned int freq[CONFIG_NOTES_COUNT] = {21, 22, 23, 25, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 74, 78, 82, 87, 93, 98, 104, 110, 117, 123, 131, 139, 148, 156, 165, 175, 185, 196, 207, 220, 233, 247, 262, 277, 293, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4698, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7458, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544, 13289, 14080, 14917, 15804, 16744, 17739, 18794, 19912};
unsigned int min_freq[CONFIG_NOTES_COUNT] = {20, 21, 22, 24, 25, 27, 28, 30, 32, 34, 36, 38, 40, 42, 45, 47, 50, 53, 56, 60, 63, 67, 71, 76, 80, 84, 90, 95, 101, 107, 113, 120, 127, 135, 143, 152, 160, 170, 180, 190, 201, 213, 226, 240, 254, 269, 285, 302, 320, 339, 359, 381, 403, 427, 453, 480, 508, 538, 570, 604, 640, 678, 719, 762, 807, 855, 906, 960, 1017, 1078, 1142, 1210, 1282, 1358, 1438, 1524, 1614, 1710, 1812, 1920, 2034, 2155, 2283, 2419, 2563, 2715, 2877, 3048, 3229, 3421, 3624, 3840, 4068, 4310, 4566, 4838, 5126, 5431, 5754, 6096, 6458, 6842, 7249, 7680, 8137, 8621, 9133, 9676, 10252, 10861, 11507, 12192, 12916, 13684, 14498, 15360, 16274, 17241, 18266, 19353};
unsigned int max_freq[CONFIG_NOTES_COUNT] = {21, 22, 24, 25, 27, 28, 30, 32, 34, 36, 38, 40, 42, 45, 47, 50, 53, 56, 60, 63, 67, 71, 76, 80, 84, 90, 95, 101, 107, 113, 120, 127, 135, 143, 152, 160, 170, 180, 190, 201, 213, 226, 240, 254, 269, 285, 302, 320, 339, 359, 381, 403, 427, 453, 480, 508, 538, 570, 604, 640, 678, 719, 762, 807, 855, 906, 960, 1017, 1078, 1142, 1210, 1282, 1358, 1438, 1524, 1614, 1710, 1812, 1920, 2034, 2155, 2283, 2419, 2563, 2715, 2877, 3048, 3229, 3421, 3624, 3840, 4068, 4310, 4566, 4838, 5126, 5431, 5754, 6096, 6458, 6842, 7249, 7680, 8137, 8621, 9133, 9676, 10252, 10861, 11507, 12192, 12916, 13684, 14498, 15360, 16274, 17241, 18266, 19353, 19956};

enum phase note[CONFIG_NOTES_COUNT] = {NOT_PRESSED};
unsigned char _step[CONFIG_NOTES_COUNT] = {0};

unsigned long _attack = 1000000;
unsigned long _decay = 1000000;
long _sustain = MAX_AMPL * 2 / 3;
unsigned long _release = 1000000;

void attack(unsigned long value) {
	_attack = constrain(value, CONFIG_MIN_ATTACK, CONFIG_MAX_ATTACK) / 100;
}

void decay(unsigned long value) {
	_decay = constrain(value, CONFIG_MIN_DECAY, CONFIG_MAX_DECAY) / 100;
}

void sustain(long value) {
	_sustain = constrain(value, 0, MAX_WAVE_AMPL);
}

void release(unsigned long value) {
	_release = constrain(value, CONFIG_MIN_RELEASE, CONFIG_MAX_RELEASE) / 100;
}

bool envelope_enable(void) {
	return timer_enable(&timer2, 1000000, &envelope_handler);
}

bool envelope_disable(void) {
	return timer_disable(timer2);
}

bool envelope_start(void) {
	return timer_start(timer2);
}

bool envelope_stop(void) {
	return timer_stop(timer2);
}

bool envelope_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx) {
	static unsigned long long count = 0;
	//Если нужно, получим новое смещение, применив эффект ВИБРАТО
	if(effect == VIBRATO)
		apply_vibrato();
	ampl_threshold = 0; //Сбросим максимальный диапазон, чтобы найти его для очередной совокупности звучащих нот
	//Найдём максимальный диапазон выходных значений как сумму максимальных (без учёта огибающей) амплитуд каждой ноты
	for (unsigned int i = 0; i < CONFIG_NOTES_COUNT; i++) {
		//Найдём левый и правый интервал между частотами конкретной ноты
		unsigned int left_detune = ((freq[i] - min_freq[i]) / CONFIG_FREQ_STEP) * _detune;
		unsigned int right_detune = ((max_freq[i] - freq[i]) / CONFIG_FREQ_STEP) * _detune;
		//Найдём кол-во волн слева и справа от несущей
		unsigned int left_waves_count = 0;
		unsigned int right_waves_count = 0;
		//Если частоты совпадают, всего одна волна
		if(left_detune)
			left_waves_count = ((freq_index(freq[i], 0) - freq_index(min_freq[i], 0)) / left_detune) * _waves_count;
		if(right_detune)
			right_waves_count = ((freq_index(max_freq[i], 0) - freq_index(freq[i], 0)) / right_detune) * _waves_count;
		unsigned long note_ampl_threshold = 0; //Макс. амплитуда ноты
		//Найдём макс. амплитуду ноты по формуле M(1 + d + d^2 + d^3 + ...), где M - максимальная амплитуда, d - спад, кол-во членов d - кол-во волн отдельно слева и справа
		unsigned int last = 1;
		for(unsigned long long i = 0; i < left_waves_count; i++) {
			note_ampl_threshold += (last *= _decrease);
		}
		last = 1;
		for(unsigned long long i = 0; i < right_waves_count; i++) {
			note_ampl_threshold += (last *= _decrease);
		}
		ampl_threshold += MAX_WAVE_AMPL * (1 + note_ampl_threshold); //Прибавим значение для очередной ноты
	}
	//Пройдёмся по всем нотам
	for (unsigned char i = 0; i < CONFIG_NOTES_COUNT; i++) {
		//Найдём левый и правый интервал между частотами конкретной ноты
		int left_detune = ((min_freq[i] - freq[i]) / CONFIG_FREQ_STEP) * _detune;
		unsigned int right_detune = ((max_freq[i] - freq[i]) / CONFIG_FREQ_STEP) * _detune;
		//Найдём кол-во волн слева и справа от несущей
		int left_waves_count = 0;
		unsigned int right_waves_count = 0;
		//Если частоты совпадают, всего одна волна
		if(left_detune)
			left_waves_count = ((freq_index(freq[i], 0) - freq_index(min_freq[i], 0)) / left_detune) * _waves_count;
		if(right_detune)
			right_waves_count = ((freq_index(max_freq[i], 0) - freq_index(freq[i], 0)) / right_detune) * _waves_count;
		switch (note[i]) {
		case ATTACK:
			if (count >= _attack / CONFIG_ENVELOPE_RESOLUTION) {
				float coef = ++_step[i] / CONFIG_ENVELOPE_RESOLUTION; //Коэффициент роста на этом шаге
				//Амплитуды всех частот до крайней левой сбросим в 0
				for (unsigned long long j = freq_index(min_freq[i], 0); j < freq_index(freq[i], left_waves_count); j++)
					ampl[j] = 0;
				ampl[freq_index(freq[i], 0)] = MAX_WAVE_AMPL * coef; //Основная частота
				//Зададим каждую из волн (учитывая шаг, все промежуточные волны сбросим в 0) слева и справа от основной как произведение предыдущей и коэффициента спада
				unsigned int last = MAX_WAVE_AMPL;
				if(left_detune) {
					for (unsigned long long j = freq_index(freq[i], 0); j >= freq_index(freq[i], left_waves_count); j--)
						if (j % left_detune == 0)
							ampl[j] = (last *= _decrease) * coef;
						else
							ampl[j] = 0;
					last = MAX_WAVE_AMPL;
				}
				if(right_detune) {
					for (unsigned long long j = freq_index(freq[i], 0); j <= freq_index(freq[i], right_waves_count); j++)
						if (j % right_detune == 0)
							ampl[j] = (last *= _decrease) * coef;
						else
							ampl[j] = 0;
				}
				//Амплитуды всех частот до крайней правой сбросим в 0
				for (unsigned long long j = freq_index(freq[i], right_waves_count + 1); j < freq_index(max_freq[i], 0); j++)
					ampl[j] = 0;
				if (_step[i] == CONFIG_ENVELOPE_RESOLUTION) { //Если дошли до максимума, сменим фазу на следующую
					_step[i] = 0;
					note[i] = DECAY;
				}
				count = 0; //Сбросим счётчик, т. к. выполнили шаг
			}
			break;
		case DECAY:
			if (count >= _decay / CONFIG_ENVELOPE_RESOLUTION) {
				float coef = 1.0 - (++_step[i] / CONFIG_ENVELOPE_RESOLUTION); //Коэффициент спада до сустейна на этом шаге
				//Амплитуды всех частот до крайней левой сбросим в 0
				for (unsigned long long j = freq_index(min_freq[i], 0); j < freq_index(freq[i], left_waves_count); j++)
					ampl[j] = 0;
				ampl[freq_index(freq[i], 0)] = _sustain + (MAX_WAVE_AMPL - _sustain) * coef; //Основная частота
				//Зададим каждую из волн (учитывая шаг, все промежуточные волны сбросим в 0) слева и справа от основной как произведение предыдущей и коэффициента спада
				unsigned int lim_last = MAX_WAVE_AMPL;
				unsigned int last = _sustain;
				if(left_detune) {
					for (unsigned long long j = freq_index(freq[i], 0); j >= freq_index(freq[i], left_waves_count); j--) {
						if(j % left_detune == 0) {
							ampl[j] = (last * _decrease) + ((lim_last - last) * _decrease) * coef;
							lim_last *= _decrease;
							last *= _decrease;
						} else {
							ampl[j] = 0;
						}
					}
				}
				if(right_detune) {
					lim_last = MAX_WAVE_AMPL;
					last = _sustain;
					for (unsigned long long j = freq_index(freq[i], 0); j <= freq_index(freq[i], right_waves_count); j++) {
						if(j % right_detune == 0) {
							ampl[j] = (last * _decrease) + ((lim_last - last) * _decrease) * coef;
							lim_last *= _decrease;
							last *= _decrease;
						} else {
							ampl[j] = 0;
						}
					}
				}
				//Амплитуды всех частот до крайней правой сбросим в 0
				for (unsigned long long j = freq_index(freq[i], right_waves_count + 1); j < freq_index(max_freq[i], 0); j++)
					ampl[j] = 0;
				if (_step[i] == CONFIG_ENVELOPE_RESOLUTION) { //Если дошли до сустейна, сменим фазу на следующую
					_step[i] = 0;
					note[i] = SUSTAIN;
				}
				count = 0; //Сбросим счётчик, т. к. выполнили шаг
			}
			break;
		case SUSTAIN:
			//Все волны поддерживаем на уровне сустейна (и с учётом спада для дополнительных волн)
			for (unsigned long long j = freq_index(min_freq[i], 0); j < freq_index(freq[i], left_waves_count); j++)
				ampl[j] = 0;
			ampl[freq_index(freq[i], 0)] = _sustain;
			unsigned int last = _sustain;
			if(left_detune) {
				for (unsigned long long j = freq_index(freq[i], 0); j >= freq_index(freq[i], left_waves_count); j--)
					if (j % left_detune == 0)
						ampl[j] = last *= _decrease;
					else
						ampl[j] = 0;
			}
			if(right_detune) {
				last = _sustain;
				for (unsigned long long j = freq_index(freq[i], 0); j <= freq_index(freq[i], right_waves_count); j++)
					if (j % right_detune == 0)
						ampl[j] = last *= _decrease;
					else
						ampl[j] = 0;
			}
			for (unsigned long long j = freq_index(freq[i], right_waves_count + 1); j < freq_index(max_freq[i], 0); j++)
				ampl[j] = 0;
			//До тех пор, пока не отпустим клавишу (не сменим извне фазу ноты на RELEASE)
			break;
		case RELEASE:
			if (count >= _release / CONFIG_ENVELOPE_RESOLUTION) {
				float coef = 1.0 - (++_step[i] / CONFIG_ENVELOPE_RESOLUTION); //Коэффициент спада до сустейна на этом шаге
				//Амплитуды всех частот до крайней левой сбросим в 0
				for (unsigned long long j = freq_index(min_freq[i], 0); j < freq_index(freq[i], left_waves_count); j++)
					ampl[j] = 0;
				ampl[freq_index(freq[i], 0)] = _sustain * coef; //Основная частота
				//Зададим каждую из волн (учитывая шаг, все промежуточные волны сбросим в 0) слева и справа от основной как произведение предыдущей и коэффициента спада
				unsigned int last = _sustain;
				if(left_detune) {
					for (unsigned long long j = freq_index(freq[i], 0); j >= freq_index(freq[i], left_waves_count); j--)
						if (j % left_detune == 0)
							ampl[j] = (last *= _decrease) * coef;
						else
							ampl[j] = 0;
				}
				if(right_detune) {
					last = _sustain;
					for (unsigned long long j = freq_index(freq[i], 0); j <= freq_index(freq[i], right_waves_count); j++)
						if (j % right_detune == 0)
							ampl[j] = (last *= _decrease) * coef;
						else
							ampl[j] = 0;
				}
				//Амплитуды всех частот до крайней правой сбросим в 0
				for (unsigned long long j = freq_index(freq[i], right_waves_count + 1); j < freq_index(max_freq[i], 0); j++)
					ampl[j] = 0;
				if (_step[i] == CONFIG_ENVELOPE_RESOLUTION) { //Если дошли до нуля, выключим ноту
					_step[i] = 0;
					note[i] = NOT_PRESSED;
				}
				count = 0; //Сбросим счётчик, т. к. выполнили шаг
			}
			break;
		default: //Нота не звучит (NOT_PRESSED)
			break;
		}
	}
	count++; //Инкрементируем счётчик
	return false; //Низкоприоритетная задача
}

unsigned int freq_index(unsigned int freq, int offset) {
	return apply_pitch_shift((freq - CONFIG_MIN_FREQ) / CONFIG_FREQ_STEP + offset);
}
