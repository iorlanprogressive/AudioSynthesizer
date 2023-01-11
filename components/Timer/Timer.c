#include "Timer.h"

gptimer_handle_t timer1 = NULL;
gptimer_handle_t timer2 = NULL;
gptimer_handle_t timer3 = NULL;
gptimer_handle_t timer4 = NULL;

bool timer_enable(gptimer_handle_t* handler, unsigned long freq, gptimer_alarm_cb_t callback) {
	const gptimer_config_t config = {
			.clk_src = GPTIMER_CLK_SRC_DEFAULT,
			.direction = GPTIMER_COUNT_UP, //Будем инкрементировать счётчик
			.resolution_hz = freq, //Частота инкрементирования значения
			.flags.intr_shared = false,
	};
	if (gptimer_new_timer(&config, &handler) != ESP_OK)
		return false;
	const gptimer_event_callbacks_t event_callback = {
			.on_alarm = callback, //Callback-функция, вызываемая при срабатывании таймера
	};
	if (gptimer_register_event_callbacks(handler, &event_callback, NULL) != ESP_OK)
		return false;
	//Т. к. инкрементируем значение с нужной частотой, перезагружать таймер будем каждый шаг
	const gptimer_alarm_config_t alarm_config = {
			.alarm_count = 1,
			.reload_count = 0,
			.flags.auto_reload_on_alarm = true,
	};
	if (gptimer_set_alarm_action(handler, &alarm_config) != ESP_OK)
		return false;
	if (gptimer_enable(handler) != ESP_OK)
		return false;
	return (gptimer_start(handler) == ESP_OK);
}

bool timer_disable(gptimer_handle_t handler) {
	if (gptimer_stop(handler) != ESP_OK)
		return false;
	if (gptimer_disable(handler) != ESP_OK)
		return false;
	if (gptimer_del_timer(handler) != ESP_OK)
		return false;
	handler = NULL;
	return true;
}

bool timer_start(gptimer_handle_t handler) {
	if (gptimer_enable(handler) != ESP_OK)
		return false;
	return (gptimer_start(handler) == ESP_OK);
}

bool timer_stop(gptimer_handle_t handler) {
	if (gptimer_stop(handler) != ESP_OK)
		return false;
	return (gptimer_disable(handler) == ESP_OK);
}
