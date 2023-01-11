#include "DelayLine.h"

long _buffer[BUFFER_LENGHT] = {0};
unsigned long long _index = 0;
float _wet = 0.5;
float _dry = 0.5;
float _feedback = 0.0;

void balance(float value) {
	_wet = constrain(value, 0.0, 1.0);
	_dry = 1.0 - _wet;
}

void feedback(float value) {
	_feedback = constrain(value, 0.0, 1.0);
}

void apply_delay_line(long* value, unsigned long delay) {
	long stored = _buffer[_index]; //Получаем ранее задержанный сигнал, который сейчас должен быть подмешан к основному
	_buffer[_index] = 0; //На место полученного сигнала ставим 0
	unsigned long long delay_index = (_index + DELAY_INDEX(delay)) % BUFFER_LENGHT; //Вычисляем индекс, куда должны будем поставить актуальный сигнал (текущий индекс + смещение)
	if (_buffer[delay_index]) { //Если тут уже есть сигнал, который был задержан на другое время (мы его ещё не "вытаскивали")
		_buffer[delay_index] += ((long)(*value + stored * _feedback)) >> 2; //Подмешаем к основному сигналу ранее задержанный и вновь поставим на задержку (ранее задержанный плавно сводится на нет коэффициентом обратной связи)
		_buffer[delay_index] >>= 2; //Среднее арифметическое, чтобы из-за накопления сигналов не выйти за допустимые пределы
	} else { //На месте, куда будем ставить задержаный сигнал, нет сигнала, можем не находить среднее арифметическое, чтобы не уменьшать громкость (амплитуду)
		_buffer[delay_index] = ((long)(*value + stored * _feedback)) >> 2; //Подмешаем к основному сигналу ранее задержанный и вновь поставим на задержку (ранее задержанный плавно сводится на нет коэффициентом обратной связи)
	}
	if (++_index == BUFFER_LENGHT) _index = 0; //Перемещаем индекс на следующий, но не выходим за пределы буфера
	*value = _dry * *value + _wet * stored; //Подмешиваем к основному сигналу ранее задержанный
}
