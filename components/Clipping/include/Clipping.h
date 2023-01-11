/*
 * Эффект КЛИППИНГ (ДИСТОРШН / ОВЕРДРАЙВ)
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

//#include <stdlib.h>
#include <stdbool.h> //Поддержка типа bool для языка C
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool use_clipping; //Флаг использования эффекта
extern unsigned long _threshold; //Порог. Значения, большие по модулю, будут приведены к нему
extern float _smoothness; //Плавность среза

void threshold(unsigned long value); //Задать порог [0, 2147483647]

void smoothness(float value); //Задать плавность [0.0, 1.0] (0.0 - эффект ДИСТОРШН - срез по порогу, 1.0 - отсутсвие эффекта, значения в интервале - эффект ОВЕРДРАЙВ)

void apply_clipping(long* value); //Применить к значению, переданному по ссылке

#ifdef __cplusplus
}
#endif
