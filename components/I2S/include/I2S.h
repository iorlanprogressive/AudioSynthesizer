/*
 * ЦАП (шина I2S)
 */

#pragma once

#include "sdkconfig.h" //Параметры

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> //Поддержка типа bool для языка C

//Заголовки нативных функций шины I2S в ESP-IDF
#include <driver/i2s_std.h>
#include <driver/i2s_common.h>
#include <driver/i2s_types.h>
#include <driver/gpio.h>
//RTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/ringbuf.h>

extern i2s_chan_handle_t _i2s_handler; //Обработчик шины
extern i2s_std_config_t _i2s_config; //Структура конфигурации шины
extern i2s_slot_mode_t _slot_mode; //Режим слота (моно / стерео)
extern unsigned long _samplerate; //Частота дискретизации (Гц)
extern i2s_data_bit_width_t _bitdepth; //Глубина дискретизации (8, 16, 24, 32 бит)
extern RingbufHandle_t _i2s_buffer; //Буфер данных
extern TaskHandle_t _i2s_task; //Задача обработчика

bool i2s_enable(bool stereo, unsigned long samplerate, unsigned char bitdepth);  //Инициализация шины и запуск (параметры см. выше)
bool i2s_disable(void); //Остановка и деинициализация шины
bool samplerate(unsigned long samplerate); //Частота дискретизации
bool bitdepth(unsigned char bitdepth); //Глубина дискретизации
bool stereo(bool stereo); //Стерео режим
bool slot(bool stereo, unsigned char bitdepth); //Конфигурация слота (стерео режим, глубина дискретизации)
bool config(bool stereo, unsigned long samplerate, unsigned char bitdepth); //Конфигурация (параметры как в инициализации)
bool i2s_start(void);
bool i2s_stop(void);
bool write(const unsigned char* data, unsigned long long size); //Отправить данные на ЦАП (указатель на массив, размер)

//Служебные функуии
void i2s_handler(void* arg); //Обработчик
unsigned long long i2s_write_data(const unsigned char *data, unsigned long long size); //Отправка данных, вызывается в обработчике
bool apply_i2s_config(void); //Применение параметров конфигурации, вызывается автоматически при изменении значения
bool set_bitdepth_val(unsigned char bitdepth); //Перевод числового значения глубины дискретизации (и проверка его соответсвия допустимым значениям)

#ifdef __cplusplus
}
#endif
