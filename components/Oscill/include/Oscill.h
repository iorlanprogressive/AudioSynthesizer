/*
 * Осциллятор (генератор несущей волны)
 * Тип волны - синусоида
 * Управляется заданием амплитуд нужным частотам в массива ampl
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#define OSC_PHASE_COEF (2 * M_PI / CONFIG_OSC_RESOLUTION) //Часть [0, 2п] выбранная так, чтобы весь "прогон" волны (разрешение) укладывался в один промежуток)

#include "Timer.h" //Используем таймер

//Используем эффекты
#include "Delay.h"
#include "Flanger.h"
#include "Tremolo.h"
#include "Clipping.h"
#include "VolumeAdjust.h"

#include "I2S.h" //Используем ЦАП для вывода звука

#include <stdbool.h> //Поддержка типа bool для языка C
#include <math.h>

enum effect_value {NONE, DELAY, FLANGER, TREMOLO, VIBRATO}; //Возможные эффекты
extern enum effect_value effect; //Используемый эффект

extern float _waves_count; //Кол-во дополнительных волн (слева и справа от несущей для ноты)
extern float _detune; //Интервал между частотами одной ноты
extern float _decrease; //Спад дополнительных частот ноты
extern unsigned int ampl[FREQ_COUNT]; //!МАССИВ УПРАВЛЕНИЯ! - амплитуда каждой частоты
extern unsigned long ampl_threshold;

void waves_count(unsigned long long value);

void detune(float value);

void decrease(float value);

bool oscil_enable(void);

bool oscil_disable(void);

bool oscil_start(void);

bool oscil_stop(void);

bool oscil_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx); //Обработчик осциллятора

#ifdef __cplusplus
}
#endif
