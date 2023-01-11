/*
 * Линия задержки (буфер)
 * Используется эффектами ДИЛЭЙ / ЭХО, ФЛЭНДЖЕР
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_LENGHT ((unsigned long long)(OSC_FREQ * (float)(CONFIG_MAX_DELAY / 1000000))) //Длина буфера (для макс. задержки)
#define DELAY_INDEX(delay) ((unsigned long long)(OSC_FREQ * (float)(delay / 1000000)) % BUFFER_LENGHT) //Определение индекса для конкретного времени задержки (считаем от нуля, смещение прибавляем в конкретной функции)

extern long _buffer[BUFFER_LENGHT]; //Буфер
extern unsigned long long _index; //Текущий индекс в буфере
extern float _wet; //Вес обработанного ("сырого") сигнала
extern float _dry; //Вес необработанного ("сухого") сигнала
extern float _feedback; //Коэффициент обратной связи - определяет количество повторов для эффекта ЭХО (если > 0 - эффект ЭХО, в противном случае эффект ДИЛЭЙ)

void balance(float value); //Баланс [0.0, 1.0] (0.0 - "сухой" сигнал, 1.0 - "сырой" сигнал)
void feedback(float value); //Уровень обтраной связи
void apply_delay_line(long* value, unsigned long delay); //Задержать сигнал value на время delay (мкс) и значение value смешать с ранее задержанными сигналами

#ifdef __cplusplus
}
#endif
