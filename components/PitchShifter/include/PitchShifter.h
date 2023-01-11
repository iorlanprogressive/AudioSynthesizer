/*
 * Питч-шифтер
 * Изменят высоту звучания (сдвигает все частоты влево или вправо)
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

extern int _shift; //Сдвиг частот

void shift(float value);
unsigned int apply_pitch_shift(unsigned int value); //Получить новый индекс в массиве частот с учётом сдвига

#ifdef __cplusplus
}
#endif
