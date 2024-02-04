/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "string.h"

/*
Set Communication Parameters - Setting baud rate, data bits, stop bits, etc.

Set Communication Pins - Assigning pins for connection to a device

Install Drivers - Allocating ESP32's resources for the UART driver

Run UART Communication - Sending/receiving data
*/

static void echo_task(void *arg)
{
    const uart_port_t uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 2400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = UART_SCLK_DEFAULT,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

    // Write data to UART.
    char* test_str = "This is a test string.\n\r";

    uint8_t data[128];

    while(1){
        //uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));
        int len = uart_read_bytes(uart_num, data, 128-1, 10);
        uart_write_bytes(uart_num, (const char*)data, len);
        if (len) {
            data[len] = '\0';
            ESP_LOGI("UART TEST", "Recv str: %s", (char *) data);
        }
    }
}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", CONFIG_EXAMPLE_TASK_STACK_SIZE, NULL, 10, NULL);
}