/*
 * Таймер
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> //Поддержка типа bool для языка C
#include <driver/gptimer.h> //Заголовки нативных функций таймера в ESP-IDF


//Доступные таймеры
extern gptimer_handle_t timer1;
extern gptimer_handle_t timer2;
extern gptimer_handle_t timer3;
extern gptimer_handle_t timer4;

bool timer_enable(gptimer_handle_t* handler, unsigned long freq, gptimer_alarm_cb_t callback); //Активация таймера (таймер, частота, callback-функция)
bool timer_disable(gptimer_handle_t handler);
bool timer_start(gptimer_handle_t handler);
bool timer_stop(gptimer_handle_t handler);

#ifdef __cplusplus
}
#endif
