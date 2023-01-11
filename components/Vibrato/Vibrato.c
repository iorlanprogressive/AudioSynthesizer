#include "Vibrato.h"

void apply_vibrato(void) {
	_shift = constrain(_shift + map(lfo_value, -127, 127, -FREQ_COUNT / 2, FREQ_COUNT / 2), -FREQ_COUNT / 2, FREQ_COUNT / 2);
}
