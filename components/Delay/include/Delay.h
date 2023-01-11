/*
 * Эффекты ДИЛЭЙ / ЭХО (повторение с задержкой)
 * ДИЛЭЙ или ЭХО (кол-во повторений) зависит от параметра обратной связи feedback (см. DelayLine)
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#include "DelayLine.h" //Используем линию задержки

extern unsigned long _delay; //Время задержки (мкс)

void delay(unsigned long value); //Установить время задержки (мкс)
void apply_delay(long* value); //Применить к значению, переданному по ссылке

#ifdef __cplusplus
}
#endif
