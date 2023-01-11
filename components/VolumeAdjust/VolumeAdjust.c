#include "VolumeAdjust.h"

float _volume = 0.5;

void volume(float value) {
	_volume = constrain(value, 0.0, 1.0);
}

void apply_volume_adjust(long* value) {
	*value *= _volume;
}
