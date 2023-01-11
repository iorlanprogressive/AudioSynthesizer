#include "PitchShifter.h"

int _shift = 0;

void shift(float value) {
	_shift = (FREQ_COUNT / 2) * constrain(value, -1.0, 1.0);
}

unsigned int apply_pitch_shift(unsigned int value) {
	return constrain(value + _shift, 0, FREQ_COUNT - 1);
}
