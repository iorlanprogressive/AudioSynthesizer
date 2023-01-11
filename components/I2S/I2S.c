#include "I2S.h"

i2s_chan_handle_t _i2s_handler;
i2s_std_config_t _i2s_config;
i2s_slot_mode_t _slot_mode;
unsigned long _samplerate;
i2s_data_bit_width_t _bitdepth;
RingbufHandle_t _i2s_buffer = NULL;
TaskHandle_t _i2s_task = NULL;

bool i2s_enable(bool stereo, unsigned long samplerate, unsigned char bitdepth) {
	if (!set_bitdepth_val(bitdepth)) //Проверка значения глубины дискретизации на корректность
		return false;
	//Установка значений
	_slot_mode = stereo ? I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;
	_samplerate = samplerate;
	i2s_chan_config_t ch_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
	ch_config.auto_clear = true; //Исключает шум, когда нет данных
	if (i2s_new_channel(&ch_config, &_i2s_handler, NULL) != ESP_OK)
		return false;
	_i2s_config = (i2s_std_config_t) {
		.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(_samplerate),
				.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(_bitdepth, _slot_mode),
				.gpio_cfg = {
						.mclk = I2S_GPIO_UNUSED,
						.bclk = CONFIG_I2S_BCLK_PIN,
						.ws = CONFIG_I2S_WS_PIN,
						.dout = CONFIG_I2S_DOUT_PIN,
						.din = I2S_GPIO_UNUSED,
						.invert_flags = {
								.mclk_inv = false,
								.bclk_inv = false,
								.ws_inv = false,
						},
		},
	};
	if (i2s_channel_init_std_mode(_i2s_handler, &_i2s_config) != ESP_OK)
		return false;
	if (i2s_channel_enable(_i2s_handler) != ESP_OK)
		return false;
	//Создаём кольцевой буфер для данных
	if ((_i2s_buffer = xRingbufferCreate(CONFIG_I2S_BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF)) == NULL)
		return false;
	//Создаём задачу-обработчик
	return (xTaskCreatePinnedToCore(i2s_handler, CONFIG_I2S_TASK_TAG, CONFIG_I2S_STACK_SIZE, NULL, CONFIG_I2S_TASK_PRIORITY, &_i2s_task, CONFIG_I2S_TASK_CORE) == pdPASS);
}

bool i2s_disable(void) {
	if (_i2s_task) {
		vTaskDelete(_i2s_task);
		_i2s_task = NULL;
	}
	if (_i2s_buffer) {
		vRingbufferDelete(_i2s_buffer);
		_i2s_buffer = NULL;
	}
	if (i2s_channel_disable(_i2s_handler) != ESP_OK)
		return false;
	return (i2s_del_channel(_i2s_handler) == ESP_OK);
}


//Устанавливаем параметры и применяем конфигурацию
bool samplerate(unsigned long samplerate) {
	_samplerate = samplerate;
	return apply_i2s_config();
}

bool bitdepth(unsigned char bitdepth) {
	if (!set_bitdepth_val(bitdepth))
		return false;
	return apply_i2s_config();
}

bool stereo(bool stereo) {
	_slot_mode = stereo ? I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;
	return apply_i2s_config();
}

bool slot(bool stereo, unsigned char bitdepth) {
	if (!set_bitdepth_val(bitdepth))
		return false;
	_slot_mode = stereo ? I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;
	return apply_i2s_config();
}

bool config(bool stereo, unsigned long samplerate, unsigned char bitdepth) {
	if (!set_bitdepth_val(bitdepth))
		return false;
	_slot_mode = stereo ? I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;
	_samplerate = samplerate;
	return apply_i2s_config();
}

bool i2s_start(void) {
	return (i2s_channel_enable(_i2s_handler) == ESP_OK);
}

bool i2s_stop(void) {
	return (i2s_channel_disable(_i2s_handler) == ESP_OK);
}

bool write(const unsigned char* data, unsigned long long size) {
	return (xRingbufferSend(_i2s_buffer, data, size, portMAX_DELAY) == pdTRUE); //Отправим данные в буфер
}

void i2s_handler(void* arg) {
	unsigned char* data = NULL;
	unsigned long long size = 0;
	while (true) { //Задача обработчика выполняется постоянно - ждёт данные для отправки на ЦАП
		//while(true) printf("123\n");
		data = (unsigned char*)xRingbufferReceive(_i2s_buffer, &size, portMAX_DELAY); //Пробуем получить данные
		if (size != 0) { //Если получили данные
			if (i2s_write_data(data, size) == size) //Отправляем данные на ЦАП и проверям корректность отправки
				vRingbufferReturnItem(_i2s_buffer, data); //Если данные были успешно отправлены, удалим их из буфера
		}
	}
}


unsigned long long i2s_write_data(const unsigned char *data, unsigned long long size) {
	unsigned long long bytes_written = 0;
	if (i2s_channel_write(_i2s_handler, data, size, &bytes_written, portMAX_DELAY) != ESP_OK) //Непосредственно отправляем данные на ЦАП
		return 0;
	return bytes_written;
}

bool apply_i2s_config(void) {
	if (i2s_channel_disable(_i2s_handler) != ESP_OK)
		return false;
	_i2s_config.clk_cfg = (i2s_std_clk_config_t)I2S_STD_CLK_DEFAULT_CONFIG(_samplerate);
	if (i2s_channel_reconfig_std_clock(_i2s_handler, &_i2s_config.clk_cfg) != ESP_OK)
		return false;
	_i2s_config.slot_cfg = (i2s_std_slot_config_t)I2S_STD_MSB_SLOT_DEFAULT_CONFIG(_bitdepth, _slot_mode);
	if (i2s_channel_reconfig_std_slot(_i2s_handler, &_i2s_config.slot_cfg) != ESP_OK)
		return false;
	return (i2s_channel_enable(_i2s_handler) == ESP_OK);
}

bool set_bitdepth_val(unsigned char bitdepth) {
	if (bitdepth == 8)
		_bitdepth = I2S_DATA_BIT_WIDTH_8BIT;
	else if (bitdepth == 16)
		_bitdepth = I2S_DATA_BIT_WIDTH_16BIT;
	else if (bitdepth == 24)
		_bitdepth = I2S_DATA_BIT_WIDTH_24BIT;
	else if (bitdepth == 32)
		_bitdepth = I2S_DATA_BIT_WIDTH_32BIT;
	else
		return false;
	return true;
}
