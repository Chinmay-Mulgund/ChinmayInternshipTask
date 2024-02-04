#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "string.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#define BUF_SIZE (1024)

static const char *TAG = "uart_echo_example";

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
    const char *long_str = "Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the last one. In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs,\" Rajan said in the note.\" Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently.\n\r";

    TickType_t start_time = xTaskGetTickCount();
    uint32_t total_bits_transmitted = 0;

    while (1)
    {
        int bytes_written = uart_write_bytes(uart_num, long_str, strlen(long_str));
        total_bits_transmitted += bytes_written * 8;
        
        // Print live real-time data transmission speed every second
        if ((xTaskGetTickCount() - start_time) >= pdMS_TO_TICKS(1000))
        {
            double elapsed_time = (xTaskGetTickCount() - start_time) * portTICK_PERIOD_MS / 1000.0;
            double data_rate = total_bits_transmitted / elapsed_time; // in bits per second
            
            ESP_LOGI(TAG, "Transmission Speed: %.2f bps", data_rate);

            // Reset counters
            start_time = xTaskGetTickCount();
            total_bits_transmitted = 0;
        }
    }
}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", CONFIG_EXAMPLE_TASK_STACK_SIZE, NULL, 10, NULL);
}
