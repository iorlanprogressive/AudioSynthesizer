#include "ADC.h"

adc_oneshot_unit_handle_t _adc_handler_sar1 = NULL;
adc_oneshot_unit_handle_t _adc_handler_sar2 = NULL;

bool adc_enable(void) {
	const adc_oneshot_unit_init_cfg_t config_sar1 = {
			.unit_id = ADC_UNIT_1, //Блок 1
			.ulp_mode = ADC_ULP_MODE_DISABLE, //Без энергосбережения
	};
	const adc_oneshot_unit_init_cfg_t config_sar2 = {
			.unit_id = ADC_UNIT_2, //Блок 2
			.ulp_mode = ADC_ULP_MODE_DISABLE, //Без энергосбережения
	};
	const adc_oneshot_chan_cfg_t ch_config = { //Для всех каналов
			.atten = ADC_ATTEN, //Затухание АЦП
			.bitwidth = ADC_BITWIDTH_DEFAULT, //Разрядность по умолчанию (12 бит)
	};
	//Сконфигурируем блоки АЦП
	if(adc_oneshot_new_unit(&config_sar1, &_adc_handler_sar1) != ESP_OK)
		return false;
	if(adc_oneshot_new_unit(&config_sar2, &_adc_handler_sar2) != ESP_OK)
		return false;
	//Сконфигурируем все каналы
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_0, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_1, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_2, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_3, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_4, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_5, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_6, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar1, ADC_CHANNEL_7, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_0, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_1, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_2, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_3, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_4, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_5, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_6, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_7, &ch_config) != ESP_OK)
		return false;
	if(adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_8, &ch_config) != ESP_OK)
		return false;
	return (adc_oneshot_config_channel(_adc_handler_sar2, ADC_CHANNEL_9, &ch_config) == ESP_OK);
}

bool adc_disable(void) {
	//Деинициализируем блоки
	if(adc_oneshot_del_unit(_adc_handler_sar1) != ESP_OK)
		return false;
	return (adc_oneshot_del_unit(_adc_handler_sar2) == ESP_OK);
}

int adc_read_value(int gpio) {
	adc_unit_t sar;
	adc_channel_t ch;
	int value;
	//Получим номер блока и номер канала, к которым подключен GPIO
	if(adc_oneshot_io_to_channel(gpio, &sar, &ch) != ESP_OK)
		return -1;
	//Прочитаем значение в переменную value
	if(adc_oneshot_read((sar == ADC_UNIT_1) ? _adc_handler_sar1 : _adc_handler_sar2, ch, &value) != ESP_OK)
		return -1;
	return value; //Возвращаем прочитанное значение (0-4095, -1 в случае неудачи)
}
