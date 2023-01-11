/*
 * ADSR-огибающая
 * Управляет осциллятором
 * Управляется установкой ноты в массиве note в фазу ATTACK при нажатии клавишы и в фазу RELEASE при отпускании
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#include "Timer.h" //Используем таймер
#include "../../Oscill/include/Oscill.h" //Используем осциллятор
#include "PitchShifter.h" //Используем питч-шифтер
#include "Vibrato.h" //Используем эффект ВИБРАТО

#include <stdbool.h> //Поддержка типа bool для языка C

//Фазы огибающей
enum phase { NOT_PRESSED,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE };

extern unsigned int freq[CONFIG_NOTES_COUNT]; //Частоты нот (Гц)
extern unsigned int min_freq[CONFIG_NOTES_COUNT]; //Мин. частота каждой ноты (Гц)
extern unsigned int max_freq[CONFIG_NOTES_COUNT]; //Макс. частота каждой ноты (Гц)

extern enum phase note[CONFIG_NOTES_COUNT]; //!МАССИВ УПРАВЛЕНИЯ! - фаза ноты
extern unsigned char _step[CONFIG_NOTES_COUNT];

//Параметры фаз
extern unsigned long _attack; //Время атака(мкс)
extern unsigned long _decay; //Время спада (мкс)
extern long _sustain;
extern unsigned long _release; //Время затухания (мкс)

void attack(unsigned long value);

void decay(unsigned long value);

void sustain(long value);

void release(unsigned long value);

bool envelope_enable(void);

bool envelope_disable(void);

bool envelope_start(void);

bool envelope_stop(void);

bool envelope_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx); //Обработчик огибающей

unsigned int freq_index(unsigned int freq, int offset); //Индекс частоты в массиве осциллятора (частота, смещение)
