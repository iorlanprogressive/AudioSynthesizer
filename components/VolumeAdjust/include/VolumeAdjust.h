/*
 * Изменение громкости
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

extern float _volume; //Коэффициент громкости [0.0, 1.0]

void volume(float value);
void apply_volume_adjust(long* value); //Применить к значению, переданному по ссылке

#ifdef __cplusplus
}
#endif
