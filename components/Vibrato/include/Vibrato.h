/*
 * Эффект ВИБРАТО
 * "Дрожание" высоты звучания (модуляция частоты - периодическое изменение сдвига питч-шифтера)
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#include "PitchShifter.h" //Используем питч-шифтер
#include "LFO.h" //Используем НЧ-осциллятор

void apply_vibrato(void); //Изменить сдвиг питч-шифтера

#ifdef __cplusplus
}
#endif
