/*
 * Основной код программы
 *
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 * ПРИМЕР
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 * В примере реализовано управление звучащими нотами через терминал
 *
 * Байты в диапазоне [1, NOTES_COUNT] - соответствующая клавиша НАЖАТА
 * Байты в диапазоне [NOTES_COUNT + 1, 2 * NOTES_COUNT] - соответствующая клавиша ОТПУЩЕНА
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 */

#include "sdkconfig.h" //Параметры

//Кнопки
#define EFFECT_BTN GPIO_NUM_21 //Переключение эффектов
#define LFO_BTN GPIO_NUM_22 //Переключение типа волны НЧ-осциллятора
#define CLIPPING_BTN GPIO_NUM_23 //Активация / деактивация эффекта КЛИППИНГ

#if (CONFIG_BITDEPTH_32_BIT == y)
#define BITDEPTH 32
#elif (CONFIG_BITDEPTH_24_BIT == y)
#define BITDEPTH 24
#elif (CONFIG_BITDEPTH_16_BIT == y)
#define BITDEPTH 16
#elif (CONFIG_BITDEPTH_8_BIT == y)
#define BITDEPTH 8
#endif

//Подключим необходимые компоненты
#include "Timer.h"
#include "ADC.h"
#include "I2S.h"
#include "Oscill.h"
#include "Envelope.h"
#include "LFO.h"
#include "Delay.h"
#include "Flanger.h"
#include "Tremolo.h"
#include "Vibrato.h"
#include "Clipping.h"
#include "PitchShifter.h"
#include "VolumeAdjust.h"

#include <stdbool.h> //Поддержка типа bool для языка C

#include <driver/gpio.h> //Заголовки нативных функций GPIO в ESP-IDF
#include <driver/uart.h> //Заголовки нативных функций для работы с UART шиной в ESP-IDF

#define adc_value(value, min, max) (map(value, 0, 4095, min, max)) //Преобразование аналогового значения в заданный диапазон

//Обработчик событий интерфейса
bool handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx) {
	unsigned char note_val = 0;
	//Проверим нажатия кнопок
	static bool pressed_lfo = false;
	static bool pressed_effect = false;
	static bool pressed_clipping = false;
	if (pressed_lfo) {
		if (!gpio_get_level(LFO_BTN)) pressed_lfo = false;
	} else {
		if (gpio_get_level(LFO_BTN)) {
			pressed_lfo = true;
			switch (wave) {
			case SIN:
				wave = SQ;
				break;
			case SQ:
				wave = TRN;
				break;
			case TRN:
				wave = SAW;
				break;
			default: //SAW
				wave = SIN;
				break;
			}
		}
	}
	if (pressed_effect) {
		if (!gpio_get_level(EFFECT_BTN)) pressed_effect = false;
	} else {
		if (gpio_get_level(EFFECT_BTN)) {
			pressed_effect = true;
			switch (effect) {
			case DELAY:
				effect = FLANGER;
				break;
			case FLANGER:
				effect = TREMOLO;
				break;
			case TREMOLO:
				effect = VIBRATO;
				break;
			case VIBRATO:
				effect = NONE;
				break;
			default: //NONE
				effect = DELAY;
				break;
			}
		}
	}
	if (pressed_clipping) {
		if (!gpio_get_level(CLIPPING_BTN)) pressed_clipping = false;
	} else {
		if (gpio_get_level(CLIPPING_BTN)) {
			pressed_clipping = true;
			use_clipping = !use_clipping;
		}
	}

	//Обновим значения с потенциометров

	int value = adc_read_value(36);
	if(value != -1) //Если прочитано корректно
		waves_count(adc_value(value, 0.0, 1.0));

	value = adc_read_value(39);
	if(value != -1) //Если прочитано корректно
		detune(adc_value(value, 0.0, 1.0));

	value = adc_read_value(32);
	if(value != -1) //Если прочитано корректно
		decrease(adc_value(value, 0.0, 1.0));

	value = adc_read_value(33);
	if(value != -1) //Если прочитано корректно
		attack(adc_value(value, CONFIG_MIN_ATTACK, CONFIG_MAX_ATTACK));

	value = adc_read_value(34);
	if(value != -1) //Если прочитано корректно
		decay(adc_value(value, CONFIG_MIN_DECAY, CONFIG_MAX_DECAY));

	value = adc_read_value(35);
	if(value != -1) //Если прочитано корректно
		sustain(adc_value(value, 0, MAX_WAVE_AMPL));

	value = adc_read_value(4);
	if(value != -1) //Если прочитано корректно
		release(adc_value(value, CONFIG_MIN_RELEASE, CONFIG_MAX_RELEASE));

	value = adc_read_value(0);
	if(value != -1) //Если прочитано корректно
		rate(adc_value(value, 1.0, 3.0));

	value = adc_read_value(2);
	if(value != -1) //Если прочитано корректно
		depth(adc_value(value, 0, 127));

	value = adc_read_value(15);
	if(value != -1) //Если прочитано корректно
		balance(adc_value(value, 0.0, 1.0));

	value = adc_read_value(13);
	if(value != -1) //Если прочитано корректно
		feedback(adc_value(value, 0.0, 1.0));

	value = adc_read_value(12);
	if(value != -1) //Если прочитано корректно
		delay(adc_value(value, 0, CONFIG_MAX_DELAY));

	value = adc_read_value(14);
	if(value != -1) //Если прочитано корректно
		threshold(adc_value(value, 0, MAX_AMPL));

	value = adc_read_value(27);
	if(value != -1) //Если прочитано корректно
		smoothness(adc_value(value, 0.0, 1.0));

	value = adc_read_value(25);
	if(value != -1) //Если прочитано корректно
		shift(adc_value(value, -1.0, 1.0));

	value = adc_read_value(26);
	if(value != -1) //Если прочитано корректно
		volume(adc_value(value, 0.0, 1.0));

	//Обработаем нажатие клавиш
	if(uart_read_bytes(CONFIG_ESP_CONSOLE_UART_NUM, &note_val, 1, 100) > 0) {
		if(note_val > 0) { //Если клавиша была нажата / отпущена
			if(note_val <= CONFIG_NOTES_COUNT) //Клавиша нажата
				note[note_val - 1] = ATTACK;
			else //Клавиша отпущена
				note[note_val - CONFIG_NOTES_COUNT - 1] = RELEASE;
		}
	}
	return false; //Низкоприоритетная задача
}

void app_main(void) { //Точка входа в программу
	//Инициализируем UART шину для получения команд управления нажатием клавиш
	uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
	uart_set_baudrate(CONFIG_ESP_CONSOLE_UART_NUM, CONFIG_BAUDRATE);
	//Инициализируем GPIO (кнопки)
	gpio_set_direction(EFFECT_BTN, GPIO_MODE_INPUT);
	gpio_set_direction(LFO_BTN, GPIO_MODE_INPUT);
	gpio_set_direction(CLIPPING_BTN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(EFFECT_BTN, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(LFO_BTN, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(CLIPPING_BTN, GPIO_PULLUP_ONLY);
	//Инициализируем компоненты
	adc_enable();
	i2s_enable(CONFIG_STEREO, CONFIG_SAMPLERATE, BITDEPTH); //Параметры задаются в sdkconfig
	lfo_enable();
	oscil_enable();
	envelope_enable();
	//Привяжем обработчик событий интерфейса к таймеру
	timer_enable(&timer4, 10000, handler);
}
