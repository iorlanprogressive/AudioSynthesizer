/*
 * АЦП
 */

#pragma once

#include "../../Service.h" //Служебные глобальные константы и макросы

#ifdef __cplusplus
extern "C" {
#endif

#if (CONFIG_ADC_ATTEN_DB_0 == y)
#define ADC_ATTEN ADC_ATTEN_DB_0
#elif (CONFIG_ADC_ATTEN_DB_2_5 == y)
#define ADC_ATTEN ADC_ATTEN_DB_2_5
#elif (CONFIG_ADC_ATTEN_DB_6 == y)
#define ADC_ATTEN ADC_ATTEN_DB_6
#elif (CONFIG_ADC_ATTEN_DB_11 == y)
#define ADC_ATTEN ADC_ATTEN_DB_11
#endif

#include <stdbool.h> //Поддержка типа bool для языка C

#include <esp_adc/adc_oneshot.h> //Заголовки нативных функций АЦП в ESP-IDF

//Обработчики АЦП 1ого и 2ого блоков
extern adc_oneshot_unit_handle_t _adc_handler_sar1;
extern adc_oneshot_unit_handle_t _adc_handler_sar2;

bool adc_enable(void); //Инициализация АЦП
bool adc_disable(void); //Деинициализация АЦП
int adc_read_value(int gpio); //Чтение значения с указанного порта GPIO

#ifdef __cplusplus
}
#endif
