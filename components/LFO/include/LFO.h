/*
 * Низкочастотный осциллятор
 * Применяется для модуляции параметров
 * Максимальная частота - 3 Гц
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#define LFO_PHASE_COEF (2 * M_PI / CONFIG_LFO_RESOLUTION) //Часть [0, 2п] выбранная так, чтобы весь "прогон" волны (разрешение) укладывался в один промежуток

#include "Timer.h" //Используем таймер

#include <stdbool.h> //Поддержка типа bool для языка C
#include <math.h>

extern unsigned long _period; //Период (мкс)
extern char _ampl; //Амплитуда [0, 127]
extern char lfo_value; //Модулирующее (текущее) значение

/*
   Типы волны:

   > СИНУСОИДА (SIN)
       _____
      /     \
     /       \
    /         \
   /           \           /
                \         /
                 \       /
                  \_____/

   > ПРЯМОУГОЛЬНАЯ  (МЕАНДР) (SQ)
         ______       ___
        |      |     |
        |      |     |
    ____|      |_____|

    > ТРЕУГОЛЬНАЯ (TRN)

       /\      /
      /  \    /
     /    \  /
    /      \/

    > ПИЛООБРАЗНАЯ  (SAW)

       /|   /
      / |  /
     /  | /
    /   |/
 */

enum wave_type { SIN,
	SQ,
	TRN,
	SAW };

extern enum wave_type wave;

void rate(float value); //Частота модуляции (фактически частота волны)

void depth(char value); //Глубина модуляции (фактически амплитуда фолны) [0, 127]

bool lfo_enable(void);

bool lfo_disable(void);

bool lfo_start(void);

bool lfo_stop(void);

bool lfo_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx); //Обработчик НЧ-осциллятора

#ifdef __cplusplus
}
#endif
