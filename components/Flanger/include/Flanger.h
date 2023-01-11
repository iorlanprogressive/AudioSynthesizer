/*
 * Эффект ФЛЭНДЖЕР
 */

#pragma once

#include "sdkconfig.h" //Параметры

#ifdef __cplusplus
extern "C" {
#endif

#include "DelayLine.h" //Используем линию задержки
#include "LFO.h" //Используем низкочастотный осциллятор для модуляции времени задержки

void apply_flanger(long*); //Применить к значению, переданному по ссылке

#ifdef __cplusplus
}
#endif
