/*
 * Эффект ТРЕМОЛО
 * "Дрожание" громкости (модуляция амплитуды)
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#include "LFO.h" //Используем НЧ-осциллятор

void apply_tremolo(long* value); //Применить к значению, переданному по ссылке

#ifdef __cplusplus
}
#endif
