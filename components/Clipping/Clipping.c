#include "Clipping.h"

bool use_clipping = false;
unsigned long _threshold = MAX_AMPL;
float _smoothness = 0.0;

void threshold(unsigned long value) {
	_threshold = constrain(value, 0, MAX_AMPL);
}

void smoothness(float value) {
	_smoothness = constrain(value, 0.0, 1.0);
}

void apply_clipping(long* value) {
	if (use_clipping) //Если используем эффект
		if (abs(*value) > _threshold) //Если модуль больше порога, обрезать
			*value = copysign(_threshold + (abs(*value) - _threshold) * _smoothness, *value); //Значение обрезается по порогу с прибавлением превышения, умноженного на коэффициент (сглаживание среза), знак учитываем, так как значения signed и значения волны колеблются относительно нуля
}
