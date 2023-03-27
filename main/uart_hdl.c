#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

static QueueHandle_t uart_queue;

static xTaskHandle bt_app_uart_task_handler = NULL;

uart_port_t uart_num = UART_NUM_1;
const int uart_rcv_buf_size = 128;
const int uart_snd_buf_size = 128;

static void bt_app_uart_hdl(void *arg)
{
	const uint8_t preamble[4] = {0x41, 0x42, 0x43, 0x44};
	uint8_t s_prmbl;
	uint8_t prmbl_cnt;

	uint8_t uart_rcv_buf[uart_rcv_buf_size];
	uint8_t uart_snd_buf[uart_snd_buf_size];
	uint8_t payload_rcv_buf[uart_rcv_buf_size];
	uint8_t payload_size;

	int8_t rcv_lenght;
	uint8_t cnt_data;

	for(;;)
	{
		rcv_lenght = uart_read_bytes(uart_num, uart_rcv_buf, uart_rcv_buf_size, (portTickType)portMAX_DELAY);
		if (rcv_lenght > 4)
		{
			s_prmbl = 0;
			prmbl_cnt = 0;
			payload_size = 0;
			for (cnt_data = rcv_lenght; cnt_data > 0; cnt_data--)
			{
				if (s_prmbl == 0) {
					if (uart_rcv_buf[cnt_data] != preamble[prmbl_cnt]) {
						prmbl_cnt = 0;
					}
					else if  (prmbl_cnt >= 3)
						s_prmbl = 1;
					else
						prmbl_cnt++;
				} else
					payload_rcv_buf[payload_size++] = uart_rcv_buf[cnt_data];
			}
//			if (s_prmbl == 1)
				// todo send message command handler
		}
	}
}

void bt_app_uart_init(void)
{
	uint8_t tx_io = GPIO_NUM_9;
	uint8_t rx_io  = GPIO_NUM_10;
	uint8_t rts_io = UART_PIN_NO_CHANGE;
	uint8_t cts_io = UART_PIN_NO_CHANGE;

	uart_config_t uart_config = {
	    .baud_rate = 115200,
	    .data_bits = UART_DATA_8_BITS,
	    .parity = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
	    .rx_flow_ctrl_thresh = 122,
	};
	// Configure UART parameters
	ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_rcv_buf_size, uart_snd_buf_size, 10, &uart_queue, 0));
	ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_io, rx_io, rts_io, cts_io));

	xTaskCreate(bt_app_uart_hdl, "BtAppUartTask", 2048, NULL, configMAX_PRIORITIES - 6, &bt_app_uart_task_handler);
}
