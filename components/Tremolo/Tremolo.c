#include "Tremolo.h"

void apply_tremolo(long* value) {
	*value *= map(lfo_value, -127, 127, 0.0, 1.0);
}
