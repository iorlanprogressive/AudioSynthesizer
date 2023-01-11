/*
 * Служебные глобальные константы и макросы
 * Используются только дерективы препроцессора
 */

#pragma once

#include "sdkconfig.h" //Параметры

////Глобальные константы
//Максимальная амплитуда с учётом глубины дискретизации (максимальное значение signed типа n бит)
#if (CONFIG_BITDEPTH_32_BIT == y)
#define MAX_AMPL 2147483647
#elif (CONFIG_BITDEPTH_24_BIT == y)
#define MAX_AMPL 8388607
#elif (CONFIG_BITDEPTH_16_BIT == y)
#define MAX_AMPL 32767
#elif (CONFIG_BITDEPTH_8_BIT == y)
#define MAX_AMPL 127
#endif

#define FREQ_COUNT ((unsigned int)((CONFIG_MAX_FREQ - CONFIG_MIN_FREQ) / CONFIG_FREQ_STEP)) //Кол-во частот

#define OSC_FREQ (CONFIG_MAX_FREQ * CONFIG_OSC_RESOLUTION) //Частота работы осциллятора. Берём максимальную. За один "проход" волны нужно сгенерировать несколько значений, поэтому умножим на разрешение

#define MAX_WAVE_AMPL ((unsigned int)(MAX_AMPL / FREQ_COUNT))
////

////Макросы
#define constrain(value, min, max) (value < min ? min : value > max ? max : value) //Приведение значения к границам диапазона в случае его выхода за них
#define map(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min) //Преобразование значения из одного диапазона в другой
////
